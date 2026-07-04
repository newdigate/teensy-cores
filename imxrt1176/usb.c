/* USB device controller + EP0 CDC/standard enumeration for the MIMXRT1176.
 *
 * Ported almost verbatim from cores/teensy4/usb.c.  The ChipIdea/EHCI USB IP
 * is byte-identical to the imxrt1062 (Teensy 4), so the controller, EP0 engine
 * and transfer scheduler port unchanged; only the imxrt1062 clock/PLL/PHY
 * bring-up is replaced by the RT1176 USB-PHY-PLL sequence (usb_pll_phy_init).
 *
 * Trimmed to the CDC-ACM + standard EP0 request set.  All AUDIO/MIDI/MTP/HID/
 * MULTITOUCH/KEYBOARD/SEREMU/RAWHID/JOYSTICK/EXPERIMENTAL cases and the SOF /
 * reboot / iso / timer paths are removed for Phase 1 (enumeration only).
 *
 * Original copyright (c) 2019 PJRC.COM, LLC. (MIT-style, see teensy4 sources)
 */

#include "usb_dev.h"
#define USB_DESC_LIST_DEFINE
#include "usb_desc.h"
#include "core_pins.h" // for delay(), delayMicroseconds(), systick_millis_count
#include <string.h>

// device mode, page 3155

#if defined(NUM_ENDPOINTS)

typedef struct endpoint_struct endpoint_t;

struct endpoint_struct {
	uint32_t config;
	uint32_t current;
	uint32_t next;
	uint32_t status;
	uint32_t pointer0;
	uint32_t pointer1;
	uint32_t pointer2;
	uint32_t pointer3;
	uint32_t pointer4;
	uint32_t reserved;
	uint32_t setup0;
	uint32_t setup1;
	transfer_t *first_transfer;
	transfer_t *last_transfer;
	void (*callback_function)(transfer_t *completed_transfer);
	uint32_t unused1;
};

// EHCI/ChipIdea queue-head array.  teensy4 places this in a dedicated
// .endpoint_queue section (4 KiB aligned).  Here it lives in DMAMEM (.dmabuffers
// -> OCRAM), still 4 KiB aligned as the controller requires.
DMAMEM __attribute__ ((aligned(4096)))
endpoint_t endpoint_queue_head[(NUM_ENDPOINTS+1)*2];

// dTDs walked by the USB bus-master must live in DMAMEM (.dmabuffers -> OCRAM),
// not DTCM.  DMAMEM already carries the 'used' attribute; dTDs need 32B alignment.
DMAMEM __attribute__ ((aligned(32))) transfer_t endpoint0_transfer_data;
DMAMEM __attribute__ ((aligned(32))) transfer_t endpoint0_transfer_ack;


typedef union {
 struct {
  union {
   struct {
	uint8_t bmRequestType;
	uint8_t bRequest;
   };
	uint16_t wRequestAndType;
  };
	uint16_t wValue;
	uint16_t wIndex;
	uint16_t wLength;
 };
 struct {
	uint32_t word1;
	uint32_t word2;
 };
	uint64_t bothwords;
} setup_t;

static setup_t endpoint0_setupdata;
static uint32_t endpoint0_notify_mask=0;
static uint32_t endpointN_notify_mask=0;
volatile uint8_t usb_configuration = 0; // non-zero when USB host as configured device
volatile uint8_t usb_high_speed = 0;    // non-zero if running at 480 Mbit/sec speed
void (*usb_timer0_callback)(void) = NULL; // USB GPTIMER0 one-shot (usb_serial flush timer)
static uint8_t endpoint0_buffer[8] DMAMEM;

// staging buffer for descriptor replies (copied from flash to DMAMEM/OCRAM).
// sized for the largest descriptor: the config descriptor (CONFIG_DESC_SIZE).
static uint8_t usb_descriptor_buffer[CONFIG_DESC_SIZE] DMAMEM;

// RT1176 480 MHz USB-PHY PLL bring-up (replaces teensy4 imxrt1062 clock block)
static void usb_pll_phy_init(void);

void usb_isr(void);
static void endpoint0_setup(uint64_t setupdata);
static void endpoint0_transmit(const void *data, uint32_t len, int notify);
static void endpoint0_receive(void *data, uint32_t len, int notify);
static void endpoint0_complete(void);


static void run_callbacks(endpoint_t *ep);


static void usb_pll_phy_init(void)
{
	CCM_LPCG115_DIRECT = 1u;                                 // CLOCK_EnableClock(kCLOCK_Usb): ungate USB LPCG (index 115)
	USBPHY1_CTRL_CLR    = USBPHY_CTRL_SFTRST;                // clear soft reset (bit 31)
	USBPHY1_PLL_SIC_SET = USBPHY_PLL_SIC_PLL_REG_ENABLE;     // 0x200000
	delayMicroseconds(20);                                    // SDK requires >= 15 us
	USBPHY1_PLL_SIC_SET = USBPHY_PLL_SIC_PLL_POWER;          // 0x1000
	USBPHY1_PLL_SIC = (USBPHY1_PLL_SIC & ~USBPHY_PLL_SIC_PLL_DIV_SEL_MASK)
	                  | USBPHY_PLL_SIC_PLL_DIV_SEL(3);       // 24 MHz -> 480 MHz (mult 20)
	USBPHY1_PLL_SIC_CLR = USBPHY_PLL_SIC_PLL_BYPASS;         // 0x10000
	USBPHY1_PLL_SIC_SET = USBPHY_PLL_SIC_PLL_EN_USB_CLKS;    // 0x40
	USBPHY1_CTRL_CLR    = USBPHY_CTRL_CLKGATE;               // bit 30
	/* Wait (bounded -- design spec: "no infinite spin") for the USB-PHY PLL to
	 * lock.  On silicon the PLL locks in ~tens of microseconds; the ~1 ms bound
	 * only guards a genuine lock failure (enumeration would then fail on HW, the
	 * real arbiter).  QEMU does not model the PHY-PLL -- PLL_SIC (0xA0) reads
	 * back 0 -- so this times out under QEMU and proceeds; the ChipIdea device
	 * model still enumerates.  The 10 us delay keeps the QEMU guest-error log
	 * for the unmodeled register down to a handful of lines. */
	for (uint32_t i = 0; i < 100 && !(USBPHY1_PLL_SIC & USBPHY_PLL_SIC_PLL_LOCK); i++) {
		delayMicroseconds(10);
	}
}


void usb_init(void)
{
	static uint8_t initialized = 0;
	if (initialized) return;
	initialized = 1;

	usb_pll_phy_init();                 // RT1176 480 MHz USB PHY-PLL
	USB1_BURSTSIZE = 0x0404;
	USBPHY1_CTRL_CLR = USBPHY_CTRL_CLKGATE;
	USBPHY1_PWD = 0;
	USB1_USBMODE = USB_USBMODE_CM(2) | USB_USBMODE_SLOM;
	memset(endpoint_queue_head, 0, sizeof(endpoint_queue_head));
	endpoint_queue_head[0].config = (64 << 16) | (1 << 15);
	endpoint_queue_head[1].config = (64 << 16);
	USB1_ENDPOINTLISTADDR = (uint32_t)&endpoint_queue_head;
	USB1_USBINTR = USB_USBINTR_UE | USB_USBINTR_UEE | USB_USBINTR_URE | USB_USBINTR_SLE;
	attachInterruptVector(IRQ_USB_OTG1, &usb_isr);
	NVIC_ENABLE_IRQ(IRQ_USB_OTG1);
	USB1_USBCMD = USB_USBCMD_RS;
}


void usb_isr(void)
{
	//  Port control in device mode is only used for
	//  status port reset, suspend, and current connect status.
	uint32_t status = USB1_USBSTS;
	USB1_USBSTS = status;

	// USB_USBSTS_SLI - set to 1 when enters a suspend state from an active state
	// USB_USBSTS_SRI - set at start of frame
	// USB_USBSTS_SRI - set when USB reset detected

	if (status & USB_USBSTS_UI) {
		uint32_t setupstatus = USB1_ENDPTSETUPSTAT;
		while (setupstatus) {
			USB1_ENDPTSETUPSTAT = setupstatus;
			setup_t s;
			do {
				USB1_USBCMD |= USB_USBCMD_SUTW;
				s.word1 = endpoint_queue_head[0].setup0;
				s.word2 = endpoint_queue_head[0].setup1;
			} while (!(USB1_USBCMD & USB_USBCMD_SUTW));
			USB1_USBCMD &= ~USB_USBCMD_SUTW;
			USB1_ENDPTFLUSH = (1<<16) | (1<<0); // page 3174
			while (USB1_ENDPTFLUSH & ((1<<16) | (1<<0))) ;
			endpoint0_notify_mask = 0;
			endpoint0_setup(s.bothwords);
			setupstatus = USB1_ENDPTSETUPSTAT; // page 3175
		}
		uint32_t completestatus = USB1_ENDPTCOMPLETE;
		if (completestatus) {
			USB1_ENDPTCOMPLETE = completestatus;
			if (completestatus & endpoint0_notify_mask) {
				endpoint0_notify_mask = 0;
				endpoint0_complete();
			}
			completestatus &= endpointN_notify_mask;
			if (completestatus) {

				// transmit:
				uint32_t tx = completestatus >> 16;
				while (tx) {
					int p=__builtin_ctz(tx);
					run_callbacks(endpoint_queue_head + p * 2 + 1);
					tx &= ~(1<<p);
				}

				// receive:
				uint32_t rx = completestatus & 0xffff;
				while(rx) {
					int p=__builtin_ctz(rx);
					run_callbacks(endpoint_queue_head + p * 2);
					rx &= ~(1<<p);
				};
			}

		}
	}
	if (status & USB_USBSTS_URI) { // page 3164
		USB1_ENDPTSETUPSTAT = USB1_ENDPTSETUPSTAT; // Clear all setup token semaphores
		USB1_ENDPTCOMPLETE = USB1_ENDPTCOMPLETE; // Clear all the endpoint complete status
		while (USB1_ENDPTPRIME != 0) ; // Wait for any endpoint priming
		USB1_ENDPTFLUSH = 0xFFFFFFFF;  // Cancel all endpoint primed status
		if ((USB1_PORTSC1 & USB_PORTSC1_PR)) {
			//printf("reset\n");
		} else {
			// we took too long to respond :(
		}
		#if defined(CDC_STATUS_INTERFACE) && defined(CDC_DATA_INTERFACE)
		usb_serial_reset();
		#endif
		endpointN_notify_mask = 0;
		// TODO: Free all allocated dTDs
	}
	if (status & USB_USBSTS_TI0) {
		if (usb_timer0_callback != NULL) usb_timer0_callback();
	}
	if (status & USB_USBSTS_PCI) {
		if (USB1_PORTSC1 & USB_PORTSC1_HSP) {
			usb_high_speed = 1;
		} else {
			usb_high_speed = 0;
		}
	}
	if (status & USB_USBSTS_SLI) { // page 3165
		//printf("suspend\n");
	}
	if (status & USB_USBSTS_UEI) {
		//printf("error\n");
	}
}


static uint8_t reply_buffer[8];

static void endpoint0_setup(uint64_t setupdata)
{
	setup_t setup;
	uint32_t endpoint, dir, ctrl;
	const usb_descriptor_list_t *list;

	setup.bothwords = setupdata;
	switch (setup.wRequestAndType) {
	  case 0x0500: // SET_ADDRESS
		endpoint0_receive(NULL, 0, 0);
		USB1_DEVICEADDR = USB_DEVICEADDR_USBADR(setup.wValue) | USB_DEVICEADDR_USBADRA;
		return;
	  case 0x0900: // SET_CONFIGURATION
		usb_configuration = setup.wValue;
		// configure all other endpoints
		#if defined(ENDPOINT2_CONFIG)
		USB1_ENDPTCTRL2 = ENDPOINT2_CONFIG;
		#endif
		#if defined(ENDPOINT3_CONFIG)
		USB1_ENDPTCTRL3 = ENDPOINT3_CONFIG;
		#endif
		#if defined(ENDPOINT4_CONFIG)
		USB1_ENDPTCTRL4 = ENDPOINT4_CONFIG;
		#endif
		#if defined(CDC_STATUS_INTERFACE) && defined(CDC_DATA_INTERFACE)
		usb_serial_configure();
		#endif
		endpoint0_receive(NULL, 0, 0);
		return;
	  case 0x0880: // GET_CONFIGURATION
		reply_buffer[0] = usb_configuration;
		endpoint0_transmit(reply_buffer, 1, 0);
		return;
	  case 0x0080: // GET_STATUS (device)
		reply_buffer[0] = 0;
		reply_buffer[1] = 0;
		endpoint0_transmit(reply_buffer, 2, 0);
		return;
	  case 0x0082: // GET_STATUS (endpoint)
		endpoint = setup.wIndex & 0x7F;
		if (endpoint > 7) break;
		dir = setup.wIndex & 0x80;
		ctrl = *((uint32_t *)&USB1_ENDPTCTRL0 + endpoint);
		reply_buffer[0] = 0;
		reply_buffer[1] = 0;
		if ((dir && (ctrl & USB_ENDPTCTRL_TXS)) || (!dir && (ctrl & USB_ENDPTCTRL_RXS))) {
			reply_buffer[0] = 1;
		}
		endpoint0_transmit(reply_buffer, 2, 0);
		return;
	  case 0x0302: // SET_FEATURE (endpoint)
		endpoint = setup.wIndex & 0x7F;
		if (endpoint > 7) break;
		dir = setup.wIndex & 0x80;
		if (dir) {
			*((volatile uint32_t *)&USB1_ENDPTCTRL0 + endpoint) |= USB_ENDPTCTRL_TXS;
		} else {
			*((volatile uint32_t *)&USB1_ENDPTCTRL0 + endpoint) |= USB_ENDPTCTRL_RXS;
		}
		endpoint0_receive(NULL, 0, 0);
		return;
	  case 0x0102: // CLEAR_FEATURE (endpoint)
		endpoint = setup.wIndex & 0x7F;
		if (endpoint > 7) break;
		dir = setup.wIndex & 0x80;
		if (dir) {
			*((volatile uint32_t *)&USB1_ENDPTCTRL0 + endpoint) &= ~USB_ENDPTCTRL_TXS;
		} else {
			*((volatile uint32_t *)&USB1_ENDPTCTRL0 + endpoint) &= ~USB_ENDPTCTRL_RXS;
		}
		endpoint0_receive(NULL, 0, 0);
		return;
	  case 0x0680: // GET_DESCRIPTOR
	  case 0x0681:
		for (list = usb_descriptor_list; list->addr != NULL; list++) {
			if (setup.wValue == list->wValue && setup.wIndex == list->wIndex) {
				uint32_t datalen;
				if ((setup.wValue >> 8) == 3) {
					// for string descriptors, use the descriptor's
					// length field, allowing runtime configured length.
					datalen = *(list->addr);
				} else {
					datalen = list->length;
				}
				if (datalen > setup.wLength) datalen = setup.wLength;

				// copy the descriptor, from flash to DMAMEM
				memcpy(usb_descriptor_buffer, list->addr, datalen);
				// prep transmit
				endpoint0_transmit(usb_descriptor_buffer, datalen, 0);
				return;
			}
		}
		break;
#if defined(CDC_STATUS_INTERFACE)
	  case 0x2221: // CDC_SET_CONTROL_LINE_STATE
		if (setup.wIndex == CDC_STATUS_INTERFACE) {
			usb_cdc_line_rtsdtr_millis = systick_millis_count;
			usb_cdc_line_rtsdtr = setup.wValue;
		}
		__attribute__((fallthrough));
		// fall through to next case, to always send ZLP ACK
	  case 0x2321: // CDC_SEND_BREAK
		endpoint0_receive(NULL, 0, 0);
		return;
	  case 0x2021: // CDC_SET_LINE_CODING
		if (setup.wLength != 7) break;
		endpoint0_setupdata.bothwords = setupdata;
		endpoint0_receive(endpoint0_buffer, 7, 1);
		return;
#endif
	}
	USB1_ENDPTCTRL0 = 0x000010001; // stall
}

static void endpoint0_transmit(const void *data, uint32_t len, int notify)
{
	if (len > 0) {
		// Executing A Transfer Descriptor, page 3182
		endpoint0_transfer_data.next = 1;
		endpoint0_transfer_data.status = (len << 16) | (1<<7);
		uint32_t addr = (uint32_t)data;
		endpoint0_transfer_data.pointer0 = addr; // format: table 55-60, pg 3159
		endpoint0_transfer_data.pointer1 = addr + 4096;
		endpoint0_transfer_data.pointer2 = addr + 8192;
		endpoint0_transfer_data.pointer3 = addr + 12288;
		endpoint0_transfer_data.pointer4 = addr + 16384;
		//  Case 1: Link list is empty, page 3182
		endpoint_queue_head[1].next = (uint32_t)&endpoint0_transfer_data;
		endpoint_queue_head[1].status = 0;
		USB1_ENDPTPRIME |= (1<<16);
		while (USB1_ENDPTPRIME) ;
	}
	endpoint0_transfer_ack.next = 1;
	endpoint0_transfer_ack.status = (1<<7) | (notify ? (1 << 15) : 0);
	endpoint0_transfer_ack.pointer0 = 0;
	endpoint_queue_head[0].next = (uint32_t)&endpoint0_transfer_ack;
	endpoint_queue_head[0].status = 0;
	USB1_ENDPTCOMPLETE = (1<<0) | (1<<16);
	USB1_ENDPTPRIME |= (1<<0);
	endpoint0_notify_mask = (notify ? (1 << 0) : 0);
	while (USB1_ENDPTPRIME) ;
}

static void endpoint0_receive(void *data, uint32_t len, int notify)
{
	if (len > 0) {
		// Executing A Transfer Descriptor, page 3182
		endpoint0_transfer_data.next = 1;
		endpoint0_transfer_data.status = (len << 16) | (1<<7);
		uint32_t addr = (uint32_t)data;
		endpoint0_transfer_data.pointer0 = addr; // format: table 55-60, pg 3159
		endpoint0_transfer_data.pointer1 = addr + 4096;
		endpoint0_transfer_data.pointer2 = addr + 8192;
		endpoint0_transfer_data.pointer3 = addr + 12288;
		endpoint0_transfer_data.pointer4 = addr + 16384;
		//  Case 1: Link list is empty, page 3182
		endpoint_queue_head[0].next = (uint32_t)&endpoint0_transfer_data;
		endpoint_queue_head[0].status = 0;
		USB1_ENDPTPRIME |= (1<<0);
		while (USB1_ENDPTPRIME) ;
	}
	endpoint0_transfer_ack.next = 1;
	endpoint0_transfer_ack.status = (1<<7) | (notify ? (1 << 15) : 0);
	endpoint0_transfer_ack.pointer0 = 0;
	endpoint_queue_head[1].next = (uint32_t)&endpoint0_transfer_ack;
	endpoint_queue_head[1].status = 0;
	USB1_ENDPTCOMPLETE = (1<<0) | (1<<16);
	USB1_ENDPTPRIME |= (1<<16);
	endpoint0_notify_mask = (notify ? (1 << 16) : 0);
	while (USB1_ENDPTPRIME) ;
}


static void endpoint0_complete(void)
{
	setup_t setup;

	setup.bothwords = endpoint0_setupdata.bothwords;
#ifdef CDC_STATUS_INTERFACE
	// 0x2021 is CDC_SET_LINE_CODING
	if (setup.wRequestAndType == 0x2021 && setup.wIndex == CDC_STATUS_INTERFACE) {
		memcpy(usb_cdc_line_coding, endpoint0_buffer, 7);
	}
#endif
}

static void usb_endpoint_config(endpoint_t *qh, uint32_t config, void (*callback)(transfer_t *))
{
	memset(qh, 0, sizeof(endpoint_t));
	qh->config = config;
	qh->next = 1; // Terminate bit = 1
	qh->callback_function = callback;
}

void usb_config_rx(uint32_t ep, uint32_t packet_size, int do_zlp, void (*cb)(transfer_t *))
{
	uint32_t config = (packet_size << 16) | (do_zlp ? 0 : (1 << 29));
	if (ep < 2 || ep > NUM_ENDPOINTS) return;
	usb_endpoint_config(endpoint_queue_head + ep * 2, config, cb);
	if (cb) endpointN_notify_mask |= (1 << ep);
}

void usb_config_tx(uint32_t ep, uint32_t packet_size, int do_zlp, void (*cb)(transfer_t *))
{
	uint32_t config = (packet_size << 16) | (do_zlp ? 0 : (1 << 29));
	if (ep < 2 || ep > NUM_ENDPOINTS) return;
	usb_endpoint_config(endpoint_queue_head + ep * 2 + 1, config, cb);
	if (cb) endpointN_notify_mask |= (1 << (ep + 16));
}


void usb_prepare_transfer(transfer_t *transfer, const void *data, uint32_t len, uint32_t param)
{
	transfer->next = 1;
	transfer->status = (len << 16) | (1<<7);
	uint32_t addr = (uint32_t)data;
	transfer->pointer0 = addr;
	transfer->pointer1 = addr + 4096;
	transfer->pointer2 = addr + 8192;
	transfer->pointer3 = addr + 12288;
	transfer->pointer4 = addr + 16384;
	transfer->callback_param = param;
}

static void schedule_transfer(endpoint_t *endpoint, uint32_t epmask, transfer_t *transfer)
{
	if (endpoint->callback_function) {
		transfer->status |= (1<<15);
	}
	__disable_irq();
	// Executing A Transfer Descriptor, page 2468 (RT1060 manual, Rev 1, 12/2018)
	transfer_t *last = endpoint->last_transfer;
	if (last) {
		last->next = (uint32_t)transfer;
		if (USB1_ENDPTPRIME & epmask) goto end;
		uint32_t status, cyccnt=ARM_DWT_CYCCNT;
		do {
			USB1_USBCMD |= USB_USBCMD_ATDTW;
			status = USB1_ENDPTSTATUS;
		} while (!(USB1_USBCMD & USB_USBCMD_ATDTW) && (ARM_DWT_CYCCNT - cyccnt < 2400));
		if (status & epmask) goto end;
		endpoint->next = (uint32_t)transfer;
		endpoint->status = 0;
		USB1_ENDPTPRIME |= epmask;
		goto end;
	}
	endpoint->next = (uint32_t)transfer;
	endpoint->status = 0;
	USB1_ENDPTPRIME |= epmask;
	endpoint->first_transfer = transfer;
end:
	endpoint->last_transfer = transfer;
	__enable_irq();
}
	// ENDPTPRIME -  Software should write a one to the corresponding bit when
	//		 posting a new transfer descriptor to an endpoint queue head.
	//		 Hardware automatically uses this bit to begin parsing for a
	//		 new transfer descriptor from the queue head and prepare a
	//		 transmit buffer. Hardware clears this bit when the associated
	//		 endpoint(s) is (are) successfully primed.
	//		 Momentarily set by hardware during hardware re-priming
	//		 operations when a dTD is retired, and the dQH is updated.

	// ENDPTSTATUS - Transmit Buffer Ready - set to one by the hardware as a
	//		 response to receiving a command from a corresponding bit
	//		 in the ENDPTPRIME register.  . Buffer ready is cleared by
	//		 USB reset, by the USB DMA system, or through the ENDPTFLUSH
	//		 register.  (so 0=buffer ready, 1=buffer primed for transmit)

	//  USBCMD.ATDTW - This bit is used as a semaphore to ensure proper addition
	//		   of a new dTD to an active (primed) endpoint's linked list.
	//		   This bit is set and cleared by software.
	//		   This bit would also be cleared by hardware when state machine
	//		   is hazard region for which adding a dTD to a primed endpoint
	//		    may go unrecognized.

static void run_callbacks(endpoint_t *ep)
{
	transfer_t *first = ep->first_transfer;
	if (first == NULL) return;

	// count how many transfers are completed, then remove them from the endpoint's list
	uint32_t count = 0;
	transfer_t *t = first;
	while (1) {
		if (t->status & (1<<7)) {
			// found a still-active transfer, new list begins here
			ep->first_transfer = t;
			break;
		}
		count++;
		t = (transfer_t *)t->next;
		if ((uint32_t)t == 1) {
			// reached end of list, all need callbacks, new list is empty
			ep->first_transfer = NULL;
			ep->last_transfer = NULL;
			break;
		}
	}
	// do all the callbacks
	while (count) {
		transfer_t *next = (transfer_t *)first->next;
		ep->callback_function(first);
		first = next;
		count--;
	}
}

void usb_transmit(int endpoint_number, transfer_t *transfer)
{
	if (endpoint_number < 2 || endpoint_number > NUM_ENDPOINTS) return;
	endpoint_t *endpoint = endpoint_queue_head + endpoint_number * 2 + 1;
	uint32_t mask = 1 << (endpoint_number + 16);
	schedule_transfer(endpoint, mask, transfer);
}

void usb_receive(int endpoint_number, transfer_t *transfer)
{
	if (endpoint_number < 2 || endpoint_number > NUM_ENDPOINTS) return;
	endpoint_t *endpoint = endpoint_queue_head + endpoint_number * 2;
	uint32_t mask = 1 << endpoint_number;
	schedule_transfer(endpoint, mask, transfer);
}

uint32_t usb_transfer_status(const transfer_t *transfer)
{
	return transfer->status;
}

#else // defined(NUM_ENDPOINTS)

void usb_init(void)
{
}

#endif // defined(NUM_ENDPOINTS)

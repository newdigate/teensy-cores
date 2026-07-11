#ifndef ENET_H
#define ENET_H
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
void     enet_init(const uint8_t mac[6]);
int      enet_send_frame(const uint8_t *frame, uint16_t len); /* 0 ok, <0 timeout */
int      enet_read_frame(uint8_t *buf, uint16_t *len);        /* 1 got, 0 none, <0 err */
uint16_t enet_mdio_read(uint8_t phy, uint8_t reg);            /* Task 4 */
void     enet_mdio_write(uint8_t phy, uint8_t reg, uint16_t v);/* Task 4 */
int      enet_phy_link_up(uint32_t timeout_ms);               /* Task 4 */
#ifdef __cplusplus
}
#endif
#endif

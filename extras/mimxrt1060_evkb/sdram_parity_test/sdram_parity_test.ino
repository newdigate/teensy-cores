// EVKB SDRAM core-integration parity test. The CORE brings up the SEMC SDRAM;
// this sketch does NOT touch SEMC. Output on Serial6 (115200), repeated in loop()
// so a serial capture always catches it. The boot-time zero check runs once.
#include <Arduino.h>

extern "C" {
  extern uint8_t external_psram_size;
  void *extmem_malloc(size_t size);
}

EXTMEM uint32_t exglobal[4096];   // static EXTMEM global -> .bss.extram @ 0x80000000

uint32_t bootNonzero;   // count of non-zero words in exglobal at boot (want 0)
uint32_t *heap;

void setup() {
  Serial6.begin(115200);
  uint32_t t = millis();
  while (!Serial6 && (millis() - t) < 2000) ;
  // Boot-time zero check BEFORE any write. Read real SDRAM, not stale cache.
  arm_dcache_delete(exglobal, sizeof(exglobal));
  bootNonzero = 0;
  for (uint32_t i = 0; i < 4096; i++) if (exglobal[i] != 0) bootNonzero++;
  heap = (uint32_t *)extmem_malloc(1024 * 1024 * sizeof(uint32_t)); // 4 MB, once
}

void loop() {
  Serial6.println("\n=== EVKB SDRAM core-integration parity test ===");
  bool inSdram = ((uint32_t)exglobal >> 28) == 8;
  Serial6.printf("external_psram_size=%u  exglobal@0x%08X inSdram=%d  bootNonzero=%lu (want 0)\n",
                 external_psram_size, (uint32_t)exglobal, inSdram, (unsigned long)bootNonzero);

  // EXTMEM global write/read.
  for (uint32_t i = 0; i < 4096; i++) exglobal[i] = i * 2654435761u;
  arm_dcache_flush_delete(exglobal, sizeof(exglobal));
  uint32_t gerr = 0;
  for (uint32_t i = 0; i < 4096; i++) if (exglobal[i] != i * 2654435761u) gerr++;

  // extmem_malloc heap write/read.
  uint32_t herr = 0; bool heapInSdram = false;
  if (heap) {
    heapInSdram = ((uint32_t)heap >> 28) == 8;
    const uint32_t N = 1024 * 1024;
    for (uint32_t i = 0; i < N; i++) heap[i] = ~i;
    arm_dcache_flush_delete(heap, N * sizeof(uint32_t));
    for (uint32_t i = 0; i < N; i++) if (heap[i] != ~i) herr++;
  }
  Serial6.printf("EXTMEM gerr=%lu  heap=%p heapInSdram=%d herr=%lu\n",
                 (unsigned long)gerr, (void *)heap, heapInSdram, (unsigned long)herr);

  bool pass = inSdram && (bootNonzero == 0) && (gerr == 0) &&
              heap && heapInSdram && (herr == 0) && (external_psram_size == 32);
  Serial6.printf("RESULT: %s\n", pass ? "PARITY PASS" : "PARITY FAIL");
  Serial6.flush();
  delay(2000);
}

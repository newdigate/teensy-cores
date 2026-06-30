#include "../imxrt1176.h"
void pinMode(unsigned char, unsigned char); void digitalWrite(unsigned char, unsigned char);
int main(void){ pinMode(13,1); for(;;){ digitalWrite(13,1); for(volatile int v=0;v<200000;v++); digitalWrite(13,0); for(volatile int v=0;v<200000;v++);} }

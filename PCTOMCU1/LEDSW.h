// UART.h
// Runs on LM3S811, LM3S1968, LM3S8962, LM4F120, TM4C123
// Simple device driver for Port F.

#define PERIOD 20000 // 50 MHz

extern volatile unsigned long High_Trigger,Low_Trigger;
void LEDSW_Init(void);


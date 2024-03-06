#include "tm4c123gh6pm.h"
#include "LEDSW.h"

volatile unsigned long High_Trigger,Low_Trigger;
#define NVIC_EN0_PORTF 0x40000000

void LEDSW_Init(void){
  SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOF; // (a) activate clock for port F
	while ((SYSCTL_RCGC2_R &= SYSCTL_RCGC2_GPIOF)==0){};
		
  GPIO_PORTF_LOCK_R = GPIO_LOCK_KEY; // 2) unlock PortF PF0  
  GPIO_PORTF_CR_R |= 0x1F;           // allow changes to PF4,1,0       
  GPIO_PORTF_DIR_R |= 0x0E;          // (c) make LEDs outputs
  GPIO_PORTF_DIR_R &= ~0x11;
  GPIO_PORTF_AFSEL_R &= ~0x1F;       //     disable alt funct 
  GPIO_PORTF_DEN_R |= 0x1F;          //     enable digital   
  GPIO_PORTF_PCTL_R &= ~0x000FFFFF; // configure as GPIO
  GPIO_PORTF_AMSEL_R &= ~0x1F;       //     disable analog functionality on PF
  GPIO_PORTF_PUR_R |= 0x11;     //     enable weak pull-up on PF0,4
		
  GPIO_PORTF_IS_R &= ~0x11;     // (d) PF0,4 is edge-sensitive
  GPIO_PORTF_IBE_R &= ~0x11;    //     PF0,4 is not both edges
  GPIO_PORTF_IEV_R |= 0x11;    //     PF4 rising edge event
  GPIO_PORTF_ICR_R = 0x11;      // (e) clear flag4
  GPIO_PORTF_IM_R |= 0x11;      // (f) arm interrupt on PF4
  NVIC_PRI7_R = (NVIC_PRI7_R&~0x00E00000)|0x00800000; // (g) priority 4
  NVIC_EN0_R |= NVIC_EN0_PORTF;      // (h) enable interrupt 30 in NVIC
		
		High_Trigger = PERIOD * .9;
		Low_Trigger = PERIOD - High_Trigger;
}


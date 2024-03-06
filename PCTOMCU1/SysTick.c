// SysTick.c
// Runs on TM4C123
// By Dr. Min He
// December 10th, 2018

#include <stdint.h>
#include "SysTick.h"
#include "tm4c123gh6pm.h"
#define TIMER1_MAX 0xFFFF
#define ONE_MICRO_SECOND          16     // number of machine cycles to generate 1us delay for 16MHz system clock
#define PF1       			(*((volatile unsigned long *)0x40025008))
#define PF2       			(*((volatile unsigned long *)0x40025010))
#define PF3       			(*((volatile unsigned long *)0x40025020))
#define LEDs 		(*((volatile uint32_t *)0x40025038))
#define Timer1_Max 			0xFFFF
#define PERIOD          160000
#define HALF_DUTY       80000

// Disable Timer1A

extern volatile unsigned long High_Trigger,Low_Trigger;

void SysTick_Init(void){
  NVIC_ST_CTRL_R = 0;           			// disable SysTick during setup
  NVIC_ST_RELOAD_R = HALF_DUTY - 1;     // reload value for 50% duty cycle
  NVIC_ST_CURRENT_R = 0;        			// any write to current clears it
  NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x1FFFFFFF)|0x40000000; // bit 31-29 for SysTick, set priority to 2
  NVIC_ST_CTRL_R |= NVIC_ST_CTRL_CLK_SRC + NVIC_ST_CTRL_INTEN + NVIC_ST_CTRL_ENABLE;  // enable with core clock and interrupts, start systick timer
}

void SysTick_Start(void){
  NVIC_ST_CTRL_R = 0;
  NVIC_ST_RELOAD_R = NVIC_ST_RELOAD_M; // number of counts to wait
  NVIC_ST_CURRENT_R = 0; // any value written to CURRENT clears   
	NVIC_ST_CTRL_R |= NVIC_ST_CTRL_ENABLE+NVIC_ST_CTRL_CLK_SRC; // enable SysTick with core clock
}

// Disable Systick timer
void SysTick_Stop(void){
	NVIC_ST_CTRL_R = 0;
}

// Calculate number of machine cycles elapsed
uint32_t SysTick_Get_MC_Elapsed(void){
	return NVIC_ST_RELOAD_R-NVIC_ST_CURRENT_R;
}

// Time delay using busy wait.
// This function assumes 16 MHz system clock.
void SysTick_Wait1us(uint32_t delay){
  NVIC_ST_CTRL_R = 0;
  NVIC_ST_RELOAD_R = delay*ONE_MICRO_SECOND-1; // number of counts to wait
  NVIC_ST_CURRENT_R = 0; // any value written to CURRENT clears
	NVIC_ST_CTRL_R |= NVIC_ST_CTRL_ENABLE+NVIC_ST_CTRL_CLK_SRC;
  while((NVIC_ST_CTRL_R&NVIC_ST_CTRL_COUNT)==0); // wait for count flag
  NVIC_ST_CTRL_R = 0;
}




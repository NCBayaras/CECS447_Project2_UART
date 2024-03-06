// SysTick.h
// Runs on TM4C123
// By Dr. Min He
// December 10th, 2018
 
#include <stdint.h>
#
// Time delay using busy wait.
void SysTick_Wait1us(uint32_t delay);
void SysTick_Start(void);
void SysTick_Stop(void);
void SysTick_Init(void);
uint32_t SysTick_Get_MC_Elapsed(void);

//PWM in systic handler(low and high)
//Systick



// CECS447Project2MCU1.c
// Runs on TM4C123
// Starter file for CEC447 Project 2 UART MCU1
// Min He
// September 26, 2023

#include "tm4c123gh6pm.h"
#include "PLL.h"
#include "UART0.h"
#include "UART2.h"
#include "LEDSW.h"
#include "SysTick.h"
#include <stdint.h>
#include <stdbool.h>  // for C boolean data type
#include "LEDSW.h"

#define HALF_DUTY       50000
#define MAX_STR_LEN 20

// TODO: define bit values for all Colors 
#define RED 		(0x02)
#define BLUE 		(0x04)
#define GREEN 	(0x08)
#define PURPLE 	(0x06)
#define WHITE 	(0x0E)
#define DARK 		(0x00)
#define CRAN 		(0x0C)
#define YELLOW  (0x0A)

// TODO: define all colors in the color wheel
const	uint8_t color_wheel[] = {DARK, RED, GREEN, BLUE, YELLOW, CRAN, PURPLE, WHITE};

// TODO: define bit addresses for the onboard three LEDs and two switches
#define LEDs 		(*((volatile uint32_t *)0x40025038))
#define SW1			(0x10)
#define SW2			(0x01)
	
extern void EnableInterrupts(void);
extern void WaitForInterrupt(void);
extern void DisableInterrupts(void);
void Mode1(void);
void Mode2(void);
void Mode3(void);
void Mode1Option1(void);
void Mode1Option2(void);
void Mode1Option3(void);
void Display_Menu(void);
void Mode1_Menu(void);
void Mode2_Menu(void);
char* Color_to_String();
uint32_t Str_to_UDec(uint8_t str[]);

void PWM_Init(void);

uint8_t str_idx=0;
uint8_t string[MAX_STR_LEN];  
bool end_of_str=false;
volatile bool Exit=false;

char* color;
uint8_t i = 0;
uint8_t current_mode = 0;
uint32_t brightness;

int main(void){
	DisableInterrupts();
  PLL_Init();
  UART0_Init(true, false);  // for PC<->MCU1
	UART2_Init(true, false);  // for MCU1<->MCU2
	LEDSW_Init();  // Initialize the onboard three LEDs and two push buttons
	SysTick_Init();
	EnableInterrupts();
	LEDs = DARK;
  while(1){
		// displays the main menu 
		Display_Menu();
		
		switch(string[0]){
		//switch(UART0_InChar()){
			case '1':
				Mode1();
				break;
			case '2':
				Mode2();
				break;
			case '3': 
				Mode3();
				break;
			default:
				break; 
		}
		UART0_OutCRLF();
  }
}

void Mode1(void){
		current_mode = 1;
		Mode1_Menu();
		while (!end_of_str) { // wait until the whole string is received.
			WaitForInterrupt();
		}
		end_of_str = false;
		str_idx = 0;
		switch(string[0]){
			case '1':
				Mode1Option1();
				break;
			case '2':
				Mode1Option2();
				break;
			case '3': 
				Mode1Option3();
				break;
			default:
				UART0_OutString((uint8_t *)"Invalid Input. Please try again ");
        return Mode1();
				break; 
		}
		
}

void Mode2(void){
	UART2_OutChar('2');
	current_mode = 2;
	Mode2_Menu();
	color = Color_to_String();
	UART0_OutString((uint8_t *)color);
	UART0_OutCRLF();
	while (!end_of_str) { // wait until the whole string is received.
			WaitForInterrupt();
			
		}
	end_of_str = false;
	str_idx = 0;
	UART0_OutCRLF();
}

void Mode3(void){
	UART2_OutChar('3');
	current_mode = 3;
	UART0_OutCRLF();
	UART0_OutString((uint8_t *)"Mode 3 MCU1: Chat Room ");
	UART0_OutCRLF();
	UART0_OutString((uint8_t *)"Press sw1 at any time to exit the chat room. ");
	UART0_OutCRLF();
	UART0_OutString((uint8_t *)"Please type a message end with a return ");
	UART0_OutCRLF();
	UART0_OutString((uint8_t *)"(less than 20 characters): ");
	UART0_OutCRLF();
	while (!Exit) { // Stay in this loop until Exit is true
    if (!end_of_str) {
        // Assume here you have some mechanism to process input
        WaitForInterrupt();
    } else {
        // Reset end_of_str for the next message
        end_of_str = false;
        // Optionally prompt the user again or handle the completed message here
				UART0_OutCRLF();
        UART2_OutString(string);
        
    }
}
// If the button was pressed, Exit becomes true, and you exit the loop
if (Exit) {
    // Reset the flag if you plan to use Mode3 again without resetting the MCU
    Exit = false;
    // Additional code to handle the exit, if necessary
}
UART0_OutCRLF();
}
void Mode1Option1(void){
	UART0_OutCRLF();
	end_of_str = false;
  str_idx = 0;
	UART0_OutCRLF();
	UART0_OutString((uint8_t *)"Please select a color from the following list: ");
	UART0_OutCRLF();
	UART0_OutString((uint8_t *)"d(dark),r(red),g(green),b(blue),y(yellow),c(cran),p(purple),w(white): ");
	while (!end_of_str) { // wait until the whole string is received.
			WaitForInterrupt();
		}
	end_of_str = false;
  str_idx = 0;
  UART0_OutCRLF();
  switch (string[0]) {
    case 'r':
        LEDs = RED;
        break;

    case 'g':
        LEDs = GREEN;
        break;

    case 'b':
        LEDs = BLUE;
        break;
		
		case 'y':
        LEDs = YELLOW;
        break;
		
    case 'c':
        LEDs = CRAN;
        break;

    case 'p':
        LEDs = PURPLE;
        break;

    case 'w':
        LEDs = WHITE;
        break;

    case 'd':
        LEDs = DARK;
        break;

    default:
        UART0_OutString((uint8_t *)"Invalid Input. Please try again ");
        return Mode1Option1();
}
		UART0_OutCRLF();
		color = Color_to_String();
		UART0_OutString((uint8_t *)color);
		UART0_OutString((uint8_t *)" LED is on. ");	
		return Mode1();
}
uint32_t Str_to_UDec(uint8_t str[]){
    uint32_t number=0;;
    uint8_t character,idx=0;

  character = str[idx];
  while(character != NULL){
    if((character>='0') && (character<='9')) {
      number = 10*number+(character-'0');   // this line overflows if above 2^32-1
    }
    else { // none decimal digit fond, stop converting and return previous digits
            return number;
    }
    character = str[++idx]; // get the next digit
  }
  return number;
}

void Mode1Option2(void) {
    UART0_OutCRLF();
    UART0_OutString((uint8_t *)"Please enter a decimal number from 0 to 100 followed by a return: ");

    while (!end_of_str) {
        WaitForInterrupt();
    }

    end_of_str = false;
    str_idx = 0;

    // Calculate PWM duty cycle based on user input brightness
    brightness = Str_to_UDec(string);
		
		if(brightness == 100){
			Low_Trigger = PERIOD - 1;
			High_Trigger = 0;
		}else if(brightness == 0){
			High_Trigger = PERIOD - 1;
			Low_Trigger = 0;
			
		}else{
    High_Trigger = (brightness * PERIOD) / 100;
		Low_Trigger = PERIOD - High_Trigger;
		}
		
    UART0_OutCRLF();
    UART0_OutCRLF();
    UART0_OutString((uint8_t *)color);
    UART0_OutString((uint8_t *)" LED is displayed at ");
    UART0_OutString((uint8_t *)string);
    UART0_OutString((uint8_t *)"% brightness");

    // Assuming Mode1() is a function to return to the main mode
    return Mode1();
}
void Mode1Option3(void){
	UART0_OutCRLF();
	UART0_OutCRLF();
	return;
}

void Display_Menu(void){
	UART0_OutString((uint8_t *)"Welcome to CECS 447 Project 2 - UART ");
	UART0_OutCRLF();
	UART0_OutString((uint8_t *)"MCU 1 ");
	UART0_OutCRLF();
	UART0_OutString((uint8_t *)"Main Menu ");
	UART0_OutCRLF();
	UART0_OutString((uint8_t *)"1. PC<->MCU1 LED Control ");
	UART0_OutCRLF();
	UART0_OutString((uint8_t *)"2. MCU1<->MCU2 Color Wheel. ");
	UART0_OutCRLF();
	UART0_OutString((uint8_t *)"3. PC<->MCU1<->MCU2<->PC Chat Room ");
	UART0_OutCRLF();
	UART0_OutString((uint8_t *)"Please choose a communication mode ");
	UART0_OutCRLF();
	UART0_OutString((uint8_t *)"(enter 1 or 2 or 3): ");
	while (!end_of_str) { // wait until the whole string is received.
			WaitForInterrupt();
		}		
		end_of_str = false;
		str_idx = 0;
		UART0_OutCRLF();
}

void Mode1_Menu(void){
	UART0_OutCRLF();
	UART0_OutString((uint8_t *)"Mode 1 Menu ");
	UART0_OutCRLF();
	UART0_OutString((uint8_t *)"1. Choose an LED color. ");
	UART0_OutCRLF();
	UART0_OutString((uint8_t *)"2. Change the brightness of current LED(s). ");
	UART0_OutCRLF();
	UART0_OutString((uint8_t *)"3. Exit. ");
	UART0_OutCRLF();
	UART0_OutString((uint8_t *)"Please select an option from the following list(enter 1 or 2 or 3): ");
}

void Mode2_Menu(void){
	UART0_OutCRLF();
	UART0_OutString((uint8_t *)"Mode 2 MCU1: press ^ to exit this mode ");
	UART0_OutCRLF();
	UART0_OutString((uint8_t *)"In color Wheel State.  ");
	UART0_OutCRLF();
	UART0_OutString((uint8_t *)"Please press sw2 to go through the colors in  ");
	UART0_OutCRLF();
	UART0_OutString((uint8_t *)"the following color wheel: Dark, Red, Green, ");
	UART0_OutCRLF();
	UART0_OutString((uint8_t *)"Blue, Yellow, Cran, Purple, White. ");
	UART0_OutCRLF();
	UART0_OutString((uint8_t *)"Once a color is selected, press sw1 to send  ");
	UART0_OutCRLF();
	UART0_OutString((uint8_t *)"the color to MCU2.");
	UART0_OutCRLF();
	UART0_OutString((uint8_t *)"Current color:  ");
}

char* Color_to_String(){
	switch(LEDs){
		case RED:
			i = 1;
			return "Red";
		case GREEN:
			i = 2;
			return "Green";
		case BLUE:
			i = 3;
			return "Blue";
		case YELLOW:
			i = 4;
			return "Yellow";
		case CRAN:
			i = 5;
			return "Cran";
		case PURPLE:
			i = 6;
			return "Purple";
		case WHITE:
			i = 7;
			return "White";
		case DARK:
			i = 0;
			return "Dark";
		default:
			return 0;
			break;
	}
}
void GPIOPortF_Handler(void){		
	// simple debouncing code: generate 20ms to 30ms delay
	for (uint32_t time=0;time<80000;time++) {}
	
  if(GPIO_PORTF_RIS_R & SW2)
	{
		GPIO_PORTF_ICR_R = SW2;
		if(i == 7){
			i = 0;
		}else{
			i++;
		}
		LEDs = color_wheel[i];
	}
	
	if(GPIO_PORTF_RIS_R & SW1 && current_mode == 2)
	{
		GPIO_PORTF_ICR_R = SW1;
		UART2_OutChar(LEDs);
		UART0_OutCRLF();
		UART0_OutString((uint8_t *)"Mode 2 MCU1: press ^ to exit this mode ");
		UART0_OutCRLF();
		UART0_OutString((uint8_t *)"Current color:  ");
		color = Color_to_String();
		UART0_OutString((uint8_t *)color);
		UART0_OutCRLF();
		UART0_OutString((uint8_t *)"Waiting for color code from MCU2  ");
		UART0_OutCRLF();
  }
	if(GPIO_PORTF_RIS_R & SW1 && current_mode == 3){
		GPIO_PORTF_ICR_R = SW1;
		Exit = 1;
	}
}

// Take care of Rx interrupt and ignore Tx interrupt
void UART0_Handler(void){
	uint8_t chr;
	
  if(UART0_RIS_R&UART_RIS_RXRIS){       // received one item
		if ((UART0_FR_R&UART_FR_RXFE) == 0) {
			chr = UART0_DR_R&0xFF;
			if (chr==CR){  // reach end of the string
				end_of_str=true;
		    string[str_idx]=NULL;  // add null terminator to end a C string.
			}
			else if (str_idx<(MAX_STR_LEN-1)){  // save one spot for C null terminator '\0'.
				if (chr==BS) {
          UART0_OutChar(BS);
					str_idx--;
				}
				else {
			    string[str_idx++]=chr;  // add the latest received symbol to end a C string.
					UART0_OutChar(chr);
			  }
			}
	  }
    UART0_ICR_R = UART_ICR_RXIC;        // acknowledge RX FIFO
  }
}

void UART2_Handler(void){
	if(UART2_RIS_R & UART_RIS_RXRIS){
		if ((UART2_FR_R & UART_FR_RXFE) == 0)
//			if(UART2_InChar() == 2){
			LEDs = UART2_DR_R;
//			UART0_OutCRLF();
//			color = Color_to_String(LEDs);
//			UART0_OutCRLF();
//			UART0_OutString((uint8_t *)"Mode 2 MCU1: press ^ to exit this mode ");
//			UART0_OutCRLF();
//			UART0_OutString((uint8_t *)"In color Wheel State.  ");
//			UART0_OutCRLF();
//			UART0_OutString((uint8_t *)"Please press sw2 to go through the colors in  ");
//			UART0_OutCRLF();
//			UART0_OutString((uint8_t *)"the following color wheel: Dark, Red, Green, ");
//			UART0_OutCRLF();
//			UART0_OutString((uint8_t *)"Blue, Yellow, Cran, Purple, White. ");
//			UART0_OutCRLF();
//			UART0_OutString((uint8_t *)"Once a color is selected, press sw1 to send  ");
//			UART0_OutCRLF();
//			UART0_OutString((uint8_t *)"the color to MCU2.");
//			UART0_OutCRLF();
//			UART0_OutString((uint8_t *)"Current color:  ");
//			UART0_OutString((uint8_t *)color);
//			}
//			if(UART2_InChar() == 3){
//				UART2_InChar();
//		}
		UART2_ICR_R = UART_ICR_RXIC;
	}
}
// SysTick ISR:
// 1. Implement timing control for duty cycle and non-duty cycle
// 2. utput a waveform based on current duty cycle
void SysTick_Handler(void) {
	NVIC_ST_CTRL_R &= ~NVIC_ST_CTRL_ENABLE;
    // Toggle the LED state based on the PWM duty cycle
    if(LEDs&color_wheel[i]){
			NVIC_ST_RELOAD_R = Low_Trigger - 1;
			LEDs &= ~color_wheel[i]; // AREA
		}else{
			NVIC_ST_RELOAD_R = High_Trigger - 1;
			LEDs |= color_wheel[i]; // AREA 
		}

    NVIC_ST_CURRENT_R = 0; // Reset the current value of the SysTick timer
    NVIC_ST_CTRL_R |= NVIC_ST_CTRL_ENABLE; // Restart SysTick timer
}

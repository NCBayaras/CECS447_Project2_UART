// UART.c
// Runs on TM4C123

// U0Rx (VCP receive) connected to PA0
// U0Tx (VCP transmit) connected to PA1

#include "UART0.h"
#include "tm4c123gh6pm.h"

#define NVIC_EN0_UART0 0x20     // UART0 IRQ number 5

//------------UART_Init------------
// Initialize the UART for 115,200 baud rate (assuming 50 MHz UART clock),
// 8 bit word length, no parity bits, one stop bit, FIFOs enabled
// Input: none
// Output: none
void UART0_Init(bool RxInt, bool TxInt){
  SYSCTL_RCGC1_R |= SYSCTL_RCGC1_UART0; // activate UART0
  SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOA; // activate port A
  UART0_CTL_R = 0;                      // disable UART
  UART0_IBRD_R = 54;                    // IBRD = int(50,000,000 / (16 * 57600)) = int(54.2535)
  UART0_FBRD_R = 16;                     // FBRD = int(0.2535 * 64 + 0.5) = 16.724
                                        // 8 bit word length (no parity bits, one stop bit, FIFOs)
//  UART0_LCRH_R = (UART_LCRH_WLEN_8|UART_LCRH_FEN);
  UART0_LCRH_R = UART_LCRH_WLEN_8;
	
	// take care of interrupt setup
	if ( RxInt | TxInt) {
		NVIC_PRI1_R = (NVIC_PRI1_R&~0x0000E000)|0x0000A000; // bits 15-13, priority 5
		NVIC_EN0_R = NVIC_EN0_UART0;           // enable UART0 interrupt in NVIC
		if (RxInt) {
			UART0_IM_R |= UART_IM_RXIM;         // Enable RX interrupt
		}
		
		if (TxInt) {
			UART0_IM_R |= UART_IM_TXIM;         // Enable TX interrupt
		}
	}
  UART0_CTL_R |= UART_CTL_RXE|UART_CTL_TXE|UART_CTL_UARTEN;// enable Tx, RX and UART
  GPIO_PORTA_AFSEL_R |= 0x03;           // enable alt funct on PA1-0
  GPIO_PORTA_DEN_R |= 0x03;             // enable digital I/O on PA1-0
                                        // configure PA1-0 as UART
  GPIO_PORTA_PCTL_R = (GPIO_PORTA_PCTL_R&0xFFFFFF00)+0x00000011;
  GPIO_PORTA_AMSEL_R &= ~0x03;          // disable analog functionality on PA
}

//---------------------OutCRLF---------------------
// Output a CR,LF to UART to go to a new line
// Input: none
// Output: none

void UART0_OutCRLF(void){
  UART0_OutChar(CR);
  UART0_OutChar(LF);
}

//------------UART_InChar------------
// Wait for new serial port input
// Input: none
// Output: ASCII code for key typed
unsigned char UART0_InChar(void){
  while((UART0_FR_R&UART_FR_RXFE) != 0); // wait until the receiving FIFO is not empty
  return((unsigned char)(UART0_DR_R&0xFF));
}
//------------UART_OutChar------------
// Output 8-bit to serial port
// Input: letter is an 8-bit ASCII character to be transferred
// Output: none
void UART0_OutChar(unsigned char data){
  while((UART0_FR_R&UART_FR_TXFF) != 0);
  UART0_DR_R = data;
}


//------------UART_OutString------------
// Output String (NULL termination)
// Input: pointer to a NULL-terminated string to be transferred
// Output: none
void UART0_OutString(unsigned char *pt){
  while(*pt){
    UART0_OutChar(*pt);
    pt++;
  }
	//UART0_OutChar(0); // add the null terminator
}
//------------UART_InUDec------------
// InUDec accepts ASCII input in unsigned decimal format
//     and converts to a 32-bit unsigned number
//     valid range is 0 to 4294967295 (2^32-1)
// Input: none
// Output: 32-bit unsigned number
// If you enter a number above 4294967295, it will return an incorrect value
// Backspace will remove last digit typed
uint32_t UART_InUDec(void){
uint32_t number=0, length=0;
char character;
  character = UART0_InChar();
  while(character != CR){ // accepts until <enter> is typed
// The next line checks that the input is a digit, 0-9.
// If the character is not 0-9, it is ignored and not echoed
    if((character>='0') && (character<='9')) {
      number = 10*number+(character-'0');   // this line overflows if above 4294967295
      length++;
      UART0_OutChar(character);
    }
// If the input is a backspace, then the return number is
// changed and a backspace is outputted to the screen
    else if((character==BS) && length){
      number /= 10;
      length--;
      UART0_OutChar(character);
    }
    character = UART0_InChar();
  }
  return number;
}

//-----------------------UART_OutUDec-----------------------
// Output a 32-bit number in unsigned decimal format
// Input: 32-bit number to be transferred
// Output: none
// Variable format 1-10 digits with no space before or after
void UART_OutUDec(uint32_t n){
// This function uses recursion to convert decimal number
//   of unspecified length as an ASCII string
  if(n >= 10){
    UART_OutUDec(n/10);
    n = n%10;
  }
  UART0_OutChar(n+'0'); /* n is between 0 and 9 */
}
//------------UART_InString------------
// Accepts ASCII characters from the serial port
//    and adds them to a string until <enter> is typed
//    or until max length of the string is reached.
//    when max length is reach, no more input will be accepted
//    the display will wait for the <enter> key to be pressed.
// It echoes each character as it is inputted.
// If a backspace is inputted, the string is modified
//    and the backspace is echoed
// terminates the string with a null character
// uses busy-waiting synchronization on RDRF
// Input: pointer to empty buffer, size of buffer
// Output: Null terminated string
// -- Modified by Agustinus Darmawan + Mingjie Qiu --
void UART0_InString(unsigned char *bufPt, unsigned short max) {
int length=0;
char character;
  character = UART0_InChar();
  while(character != CR){
    if(character == BS){ // back space
      if(length){
        bufPt--;
        length--;
        UART0_OutChar(BS);
      }
    }
    else if(length < max){
      *bufPt = character;
      bufPt++;
      length++;
      UART0_OutChar(character);
    }
    character = UART0_InChar();
  }
  *bufPt = 0; // adding null terminator to the end of the string.
}


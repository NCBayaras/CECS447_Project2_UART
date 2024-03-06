// UART2.h
// Runs on LM3S811, LM3S1968, LM3S8962, LM4F120, TM4C123
// Simple device driver for UART2.
// Daniel Valvano
// September 11, 2013
// Modified by EE345L students Charlie Gough && Matt Hawk
// Modified by EE345M students Agustinus Darmawan && Mingjie Qiu

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2013
   Program 4.12, Section 4.9.4, Figures 4.26 and 4.40

 Copyright 2013 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

#include <stdint.h>
#include <stdbool.h>  // for C boolean data type

// U2Rx connected to PD6
// U2Tx connected to PD7
// standard ASCII symbols
#define CR   0x0D
#define LF   0x0A
#define BS   0x08
#define ESC  0x1B
#define SP   0x20
#define DEL  0x7F
#define NULL 0

//------------UART2_Init------------
// Initialize UART2 for 115,200 baud rate (assuming 50 MHz clock),
// 8-bit word length, no parity bits, one stop bit, FIFOs enabled
// Input: none
// Output: none
void UART2_Init(bool RxInt, bool TxInt);

//---------------------UART2_OutCRLF---------------------
// Output a CR,LF to UART2 to go to a new line
// Input: none
// Output: none
void UART2_OutCRLF(void);

//------------UART2_InChar------------
// Wait for new serial port input on UART2
// Input: none
// Output: ASCII code for key typed
unsigned char UART2_InChar(void);

//------------UART2_OutChar------------
// Output 8-bit to serial port UART2
// Input: letter is an 8-bit ASCII character to be transferred
// Output: none
void UART2_OutChar(unsigned char data);

//------------UART2_OutString------------
// Output String (NULL termination) on UART2
// Input: pointer to a NULL-terminated string to be transferred
// Output: none
void UART2_OutString(unsigned char *pt);

//------------UART2_InString------------
// Accepts ASCII characters from the serial port UART2
// and adds them to a string until <enter> is typed
// or until the max length of the string is reached.
// When the max length is reached, no more input will be accepted,
// and the display will wait for the <enter> key to be pressed.
// It echoes each character as it is inputted.
// If a backspace is inputted, the string is modified,
// and the backspace is echoed.
// Terminates the string with a null character.
// Uses busy-waiting synchronization on RDRF.
// Input: pointer to an empty buffer, size of buffer
// Output: Null-terminated string
// -- Modified by Agustinus Darmawan + Mingjie Qiu --
void UART2_InString(unsigned char *bufPt, unsigned short max);

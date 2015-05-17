#ifndef SERIAL_H_
#define SERIAL_H_
// + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
// + AUTHOR      : Guido Trensch, 2012
// + FILE        : serial128.h
// + DESCRIPTION : serial128.asm C-interface wrapper
// +
// +               Provides following functions:
// +
// +               InitUSART0 ( none          )
// +               SendString ( char* pString )
// + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +

#include <stdio.h>
#include <string.h>

//
// F U N C T I O N S
//

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// WRAPPER     :  InitUSART0()
// ROUTINE     :  ASM_InitUSART0
// DESCRIPTION :  Initialize USART0
// PARAMETERS  :  none
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
extern void* ASM_InitUSART0();
#define InitUSART0 ASM_InitUSART0

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// WRAPPER     :  Debug( char* pString )
// ROUTINE     :  ASM_SendString
// DESCRIPTION :  Sends a string via USART0
// PARAMETERS  :  pString ... pointer to C-string with max length of 255
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
extern void* ASM_SendString();                   // prototype

volatile char*    glob_pData = NULL;             // global variable, pointer string
volatile uint8_t  glob_lenData = 0x00;           // global variable, len string
                                                 // function wrapper
#if( _DEBUG )
#define Debug( X )                         \
{                                          \
  char   send_buffer[256] = { 0 };         \
  char*  pBuffer = &send_buffer[0];        \
  sprintf( pBuffer, "%s   END", X );       \
  glob_pData = pBuffer;                    \
  glob_lenData = strnlen( pBuffer, 255 );  \
  ASM_SendString();                        \
}
#else
#define Debug( X )                         \
{                                          \
}
#endif

#endif  // SERIAL_H_

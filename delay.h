#ifndef DELAY_H_
#define DELAY_H_
// + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
// + AUTHOR      : Guido Trensch, 2012
// + FILE        : delay.h
// + DESCRIPTION : delay.asm C-interface wrapper
// +
// +               Provides following functions:
// +
// +               Delay_s ( seconds )
// +               Delay_ms( milliseconds )
// +               Delay_us( microseconds )
// + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +

#include <avr/interrupt.h>

//
// F U N C T I O N S
//

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// WRAPPER     :  Delay_s( seconds )
// ROUTINE     :  ASM_Delay_Seconds
// DESCRIPTION :  Loop for n seconds.
// PARAMETERS  :  seconds = loop time in seconds
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
extern void* ASM_Delay_Seconds();                // prototype

volatile int glob_seconds;                       // global variable
                                                 // function wrapper
#define Delay_s( X )         \
{                            \
  glob_seconds = X;          \
  ASM_Delay_Seconds();       \
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// WRAPPER     :  Delay_ms( milliseconds )
// ROUTINE     :  ASM_Delay_Millis
// DESCRIPTION :  Loop for n milli seconds.
// PARAMETERS  :  milliseconds = loop time in milli seconds
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
extern void* ASM_Delay_Millis();                 // prototype

volatile int glob_milliseconds;                  // global variable
                                                 // function wrapper
#define Delay_ms( X )        \
{                            \
  glob_milliseconds = X;     \
  ASM_Delay_Millis();        \
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// WRAPPER     :  Delay_us( microseconds )
// ROUTINE     :  ASM_Delay_Micros
// DESCRIPTION :  Loop for n micro seconds.
// PARAMETERS  :  microseconds = loop time in micro seconds
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
extern void* ASM_Delay_Micros();                 // prototype

volatile int glob_microseconds;                  // global variable
                                                 // function wrapper
#define Delay_us( X )        \
{                            \
  glob_microseconds = X;     \
  ASM_Delay_Micros();        \
}
#endif  // DELAY_H_

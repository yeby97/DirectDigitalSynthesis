#ifndef ENCODER_H_
#define ENCODER_H_
// + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
// + AUTHOR      : Guido Trensch, 2012
// + FILE        : encoder.h
// + DESCRIPTION : encoder.asm C-interface wrapper
// +
// +               Provides following functions:
// +
// +               InitEncoder( _PTR_FUNC_ pCallbackFuncSW
// +                          , _PTR_FUNC_ pCallbackFuncAB_Left
// +                          , _PTR_FUNC_ pCallbackFuncAB_Right )
// +               LockEncAB()
// +               ReleaseEncAB()
// + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +

#include <avr/interrupt.h>

//
// F U N C T I O N S
//

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// WRAPPER     :  InitEncoder( _PTR_FUNC_ pCallbackFuncSW
//                           , _PTR_FUNC_ pCallbackFuncAB_Left
//                           , _PTR_FUNC_ pCallbackFuncAB_Right )
// ROUTINE     :  ASM_InitEncoder
// DESCRIPTION :  Registers 3 callback functions and initilizes the encoder
//                The callback functions are called via an interrut in case the
//                encoder switch is pressed or turned respectively.
// PARAMETERS  :  pCallbackFuncSW       = callback function pointer
//                pCallbackFuncAB_Left  = callback function pointer
//                pCallbackFuncAB_Right = callback function pointer
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
extern void* ASM_InitEncoder();                  // prototype
typedef void*(*_PTR_FUNC_)();                    // function pointer type definition

volatile _PTR_FUNC_ glob_pCallBackFunc_Switch;   // global variable, function pointer
volatile _PTR_FUNC_ glob_pCallBackFunc_Left;     // global variable, function pointer
volatile _PTR_FUNC_ glob_pCallBackFunc_Right;    // global variable, function pointer
                                                 // function wrapper
#define InitEncoder( X, Y, Z )         \
{                                      \
  glob_pCallBackFunc_Switch = X;       \
  glob_pCallBackFunc_Left   = Y;       \
  glob_pCallBackFunc_Right  = Z;       \
  ASM_InitEncoder();                   \
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// WRAPPER     :  LockEncAB()
// ROUTINE     :  ASM_LockEncAB
// DESCRIPTION :  Locks the encoder AB interrupt
// PARAMETERS  :  none
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
extern void* ASM_LockEncAB();                    // prototype
#define LockEncAB ASM_LockEncAB                  // function wrapper

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// WRAPPER     :  ReleaseEncAB()
// ROUTINE     :  ASM_ReleaseEncAB
// DESCRIPTION :  Releases the encoder AB interrupt
// PARAMETERS  :  none
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
extern void* ASM_ReleaseEncAB();                 // prototype
#define ReleaseEncAB ASM_ReleaseEncAB            // function wrapper

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// WRAPPER     :  SIGNAL( PCINT3_vect )
// ROUTINE     :  ASM_IniterruptEncSW
// DESCRIPTION :  Interrupt encoder switch
//                The PCINT3 interrupt will cause a call to ASM_IniterruptEncSW
//                which will forward it to the registered callback function.
// PARAMETERS  :  none
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
extern void* ASM_InterruptEncSW();              // prototype
                                                // function wrapper
SIGNAL( PCINT3_vect ) {                         // Interrupt Handler
  ASM_InterruptEncSW();
  return;                                       // implizit SEI
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// WRAPPER     :  SIGNAL( PCINT1_vect )
// ROUTINE     :  ASM_IniterruptEncAB
// DESCRIPTION :  Interrupt encoder switch
//                The PCINT1 interrupt will cause a call to ASM_IniterruptEncA
//                which will forward it to the registered callback function.
// PARAMETERS  :  none
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
extern void* ASM_InterruptEncAB();              // prototype
                                                // function wrapper
SIGNAL( PCINT1_vect ) {                         // Interrupt Handler
  ASM_InterruptEncAB();
  return;                                       // implizit SEI
}

#endif  // ENCODER_H_

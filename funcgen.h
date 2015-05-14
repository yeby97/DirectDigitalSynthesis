#ifndef FUNCGEN_H_
#define FUNCGEN_H_
// + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
// + AUTHOR      : Guido Trensch, 2012
// + FILE        : funcgen.h
// + DESCRIPTION : funcgen.asm C-interface wrapper
// +
// +               Provides following functions:
// +
// +               InitAnalogFGen ( none                              )
// +               SetAnalogMaxF  ( ENUM_FREQUENCY_RANGE fc_frequency )
// +               SetOutputLevel ( ENUM_OUTPUT_LEVEL    fc_level     )
// +               SetWaveSource  ( ENUM_WAVE_FORM       fc_waveform  )
// + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +

//
// F U N C T I O N S
//

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// WRAPPER     :  InitAnalogFGen()
// ROUTINE     :  ASM_InitAnalogFGen
// DESCRIPTION :  Initialize switches I/O lines
// PARAMETERS  :  none
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
extern void* ASM_InitAnalogFGen();
#define InitAnalogFGen ASM_InitAnalogFGen

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// WRAPPER     :  SetAnalogMaxF( ENUM_FREQUENCY_RANGE fc_frequency )
// ROUTINE     :  ASM_SetAnalogMaxF
// DESCRIPTION :  Set the function generator's maximum frequency
// PARAMETERS  :  fc_frequency = [ F100HZ, F200HZ, F1000HZ, F2000HZ, F10000HZ,
//                                 F20000HZ, F100000HZ, F200000HZ ]
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
extern void* ASM_SetAnalogMaxF();                 // prototype

typedef enum {                                    // parameter enumeration
  MIN_F     = 0,
  F100HZ    = 1,
  F200HZ    = 2,
  F1000HZ   = 3,
  F2000HZ   = 4,
  F10000HZ  = 5,
  F20000HZ  = 6,
  F100000HZ = 7,
  F200000HZ = 8,
  MAX_F
} ENUM_MAX_FREQUENCY;

volatile uint8_t  glob_fc_frequency  = F1000HZ;   // global parameter variable
volatile uint8_t  glob_max_frequency = MAX_F;     // global max parameter value
                                                  // function wrapper  
#define SetAnalogMaxF( X )               \
{                                        \
  ENUM_MAX_FREQUENCY fc_frequency = X;   \
  glob_fc_frequency = fc_frequency;      \
  ASM_SetAnalogMaxF();                   \
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// WRAPPER     :  SetOutputLevel( ENUM_OUTPUT_LEVEL fc_level )
// ROUTINE     :  ASM_SetOutputLevel
// DESCRIPTION :  Set the function generator's output level
// PARAMETERS  :  fc_level = [ LVL1, LVL10, LVL100 ]
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
extern void* ASM_SetOutputLevel();               // prototype

typedef enum {                                   // parameter enumeration
  MIN_LVL = 0,
  LVLOFF  = 1,
  LVL10   = 2,
  LVL50   = 3,
  LVL100  = 4,
  MAX_LVL
} ENUM_OUTPUT_LEVEL;

volatile uint8_t  glob_fc_level  = LVL100;       // global parameter variable
volatile uint8_t  glob_max_level = MAX_LVL;      // global max parameter value
                                                 // function wrapper
#define SetOutputLevel( X )              \
{                                        \
  ENUM_OUTPUT_LEVEL fc_level = X;        \
  glob_fc_level = fc_level;              \
  ASM_SetOutputLevel();                  \
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// WRAPPER     :  SetWaveSource( ENUM_WAVE_FORM fc_waveform )
// ROUTINE     :  ASM_SetWaveSource
// DESCRIPTION :  Set the function generator's wave form
// PARAMETERS  :  fc_waveform = [ SINE, SQUARE, TRIANGLE, PULS, SAWTOOTH ]
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
extern void* ASM_SetWaveSource();                // prototype

typedef enum {                                   // parameter enumeration
  MIN_WAVE_SOURCE = 0,
  ANALOG_SINE     = 1,
  ANALOG_SQUARE   = 2,
  ANALOG_TRIANGLE = 3,
  ANALOG_PULS     = 4,
  ANALOG_SAWTOOTH = 5,
  DDS             = 6,
  NUM_WAVE_SOURCE
} ENUM_WAVE_SOURCE;

volatile uint8_t  glob_wave_form     = ANALOG_SINE;     // global parameter variable
volatile uint8_t  glob_num_wave_form = NUM_WAVE_SOURCE; // global max parameter value
                                                        // function wrapper
#define SetWaveSource( X )               \
{                                        \
  ENUM_WAVE_SOURCE wave_form = X;        \
  glob_wave_form = wave_form;            \
  ASM_SetWaveSource();                   \
}

#endif  // FUNCGEN_H_

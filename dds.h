#ifndef DDS_H_
#define DDS_H_
// + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
// + AUTHOR      : Guido Trensch, 2012
// + FILE        : dds.h
// + DESCRIPTION : dds.asm C-interface wrapper
// +
// +               Provides following functions:
// +
// +               InitDDSFGen ( ddsControl, ddsFrequency )
// +               RunDDSFgen  ()
// +               StopDDSFGen ()
// + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +

//
// F U N C T I O N S
//

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// WRAPPER     :  InitDDSFGen( ENUM_DDS_WAVE  ddsWaveForm
//                           , uint32_t       ddsFrequency )
// ROUTINE     :  ASM_Init_DDS
// DESCRIPTION :  Initialize and start the DDS generator
// PARAMETERS  :  ENUM_DDS  ddsWaveForm  ... wave form
//                uint32_t  ddsFrequency ... frequency
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
extern void* ASM_Init_DDS();

typedef enum {                                    // parameter enumeration
  DDS_STOP     = 0,
  DDS_RUN      = 1
} ENUM_DDS_CNTL;

typedef enum {                                    // parameter enumeration
  DDS_SINE     = 0,
  DDS_SQUARE   = 1,
  DDS_TRIANGLE = 2,
  DDS_SAWTOOTH = 3,
  DDS_PULS     = 4,
  DDS_STEP     = 5,
  DDS_RAMP     = 6
} ENUM_DDS_WAVE_FORM;

volatile ENUM_DDS_WAVE_FORM glob_ddsWaveForm  = DDS_SINE;  // global variables
volatile ENUM_DDS_CNTL  glob_ddsControl       = DDS_STOP;  // global variables
volatile uint32_t       glob_ddsFrequency     = 0;
                                                 // function wrapper
#define InitDDSFGen( X, Y )           \
{                                     \
  glob_ddsWaveForm  = X;              \
  glob_ddsFrequency = (Y*10906)/1000; \
  glob_ddsControl   = DDS_STOP;       \
  ASM_Init_DDS();                     \
}
// Note: The calculation "Y*10906)/1000" minimizes errors above 1000Hz. Refer also to dds.asm.
//       Below this the frequency won't be exact due to the rounding error.

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// WRAPPER     :  RunDDSFGen()
// ROUTINE     :  ASM_Run_DDS
// DESCRIPTION :  Run the DDS generator
// PARAMETERS  :  none
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
extern void* ASM_Run_DDS();
#define RunDDSFGen()                  \
{                                     \
  glob_ddsControl   = DDS_RUN;        \
  ASM_Run_DDS();                      \
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// WRAPPER     :  StopDDSFGen()
// ROUTINE     :  none
// DESCRIPTION :  Stop the DDS generator
// PARAMETERS  :  none
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
#define StopDDSFGen()                 \
{                                     \
  glob_ddsControl  = DDS_STOP;        \
}

#endif  // DDS_H_

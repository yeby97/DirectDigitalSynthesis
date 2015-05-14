// + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
// + AUTHOR      : Guido Trensch, 2012
// + FILE        : main.c
// + DESCRIPTION : Function Generator Main Entry Module
// + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +

#define _DEBUG 0

#include "delay.h"
#include "funcgen.h"
#include "serial128.h"
#include "lcd2x8.h"
#include "encoder.h"
#include "dds.h"

// + + + TYPE DEFINITIONS + + +

typedef enum {                                                                      // operation modes enumeration
  MODE_ANALOG = 0,
  MODE_DDS    = 1,
  MODE_DUAL   = 2
} ENUM_RUN_MODE;

typedef enum {                                                                      // encoder rotation direction enumeration
  ROT_NONE  = 0,
  ROT_LEFT  = 1,
  ROT_RIGHT = 2
} ENUM_ROTDIR;

typedef void*(*PTR_FUNC)();                                                         // function pointer type definition

typedef struct _MENU_ITEM
{
  char*      txtRow1;                                                               // text displayed on LCD row 1
  char*      txtRow2;                                                               // text displayed on LCD row 2
  PTR_FUNC   funcCallOnEntryRotate;                                                 // function called on menue item entry in case of rotate
  PTR_FUNC   funcCallOnEntrySelect;                                                 // function called on menue item entry in case of switch pressed
  PTR_FUNC   funcCallOnExitSelect;                                                  // function called on menue item exit in case of switch pressed
  uint8_t    prevIndex;                                                             // next menu item index on left rotate
  uint8_t    nextIndex;                                                             // next menu item index on right rotate
  uint8_t    switchIndexModeAnalog;                                                 // next menu item index on switch pressed in MODE_ANALOG
  uint8_t    switchIndexModeDDS;                                                    // next menu item index on switch pressed in MODE_DDS
  uint8_t    switchIndexModeDual;                                                   // next menu item index on switch pressed in MODE_DUAL
} MENUE_ITEM;

// + + + GLOBAL STATIC VARIABLES + + +

static ENUM_ROTDIR          rotationDirection  = ROT_NONE;
static uint32_t             ddsFrequency       = 1000;                              // DDS default frequency in hertz
static uint32_t             ddsFrequencyMax    = 100000;                            // DDS default max frequency in hertz
static uint32_t             ddsFrequencyMin    = 1;                                 // DDS default min frequency in hertz
static uint32_t             ddsFrequencyStep   = 100;                               // DDS default frequency step in hertz
static ENUM_DDS_WAVE_FORM   ddsWaveForm        = DDS_SINE;                          // DDS default wave form
static ENUM_WAVE_SOURCE     analogWaveForm     = ANALOG_SINE;                       // Analog default waveform
static ENUM_MAX_FREQUENCY   analogMaxFrequency = F1000HZ;                           // Analog default max frequency
static ENUM_OUTPUT_LEVEL    outputLevel        = LVL100;                            // default output level
static ENUM_RUN_MODE        runMode            = MODE_ANALOG;                       // default run mode
static uint8_t              menueIndex         = 1;                                 // position within the menue tree represented by the table below
                                                                                    // variable is modified by rotating or pressing the incremental encoder switch according to the table below
// + + + FUNCTION PROTOTYPES + + +

void CallBack_SwitchPressed();
void CallBack_EncLeftRotate();
void CallBack_EncRightRotate();
void ActualizeLCD();
void SetFGenProperties();
void MenuFctER_Operate();
void MenuFctES_Operate();
void MenuFctXS_Operate();
void MenuFctXS_Analog();
void MenuFctXS_DDS();
void MenuFctXS_Dual();
void MenuFctERXS_ASine();
void MenuFctERXS_ASquare();
void MenuFctERXS_ATriangle();
void MenuFctERXS_APuls();
void MenuFctERXS_ASawtooth();
void MenuFctERXS_DSine();
void MenuFctERXS_DSquare();
void MenuFctERXS_DTriangle();
void MenuFctERXS_DSawtooth();
void MenuFctERXS_DPuls();
void MenuFctERXS_DStep();
void MenuFctERXS_DRamp();
void MenuFctERXS_A100HZ();
void MenuFctERXS_A200HZ();
void MenuFctERXS_A1KHZ();
void MenuFctERXS_A2KHZ();
void MenuFctERXS_A10KHZ();
void MenuFctERXS_A20KHZ();
void MenuFctERXS_A100KHZ();
void MenuFctERXS_A200KHZ();
void MenuFctERXS_LVL100();
void MenuFctERXS_LVL50();
void MenuFctERXS_LVL10();
void MenuFctERXS_LVLOFF();
void MenuFctERXS_Operate();
void MenuFctXS_STEP1();
void MenuFctXS_STEP10();
void MenuFctXS_STEP100();
void MenuFctXS_STEP1000();
void MenuFctXS_STEP10000();

// + + + MENUE TREE TABLE + + +

// array_Menue[] defines the menue structure. Navigation through it is done by pressing and rotating the incremental encoder switch.
// The table represents a tree. Subtrees may overlay depending on runMode.
// This approach allows to add new menu items by just defining it and including the corresponding functions.

static MENUE_ITEM array_Menue[] = {
//                                                                                                                         +---------------------- next menue index on key left rotate
//                                                                                                                         |  +------------------- next menue index on key right rotate
//                                                                                                                         |  |  +---------------- next menue index on key press in MODE_ANALOG
//  menue text displayed                                                                                                   |  |  |  +------------- next menue index on key press in MODE_DDS
//  on the LCD panel                                                                                                       |  |  |  |  +---------- next menue index on key press in MODE_DUAL
//  LCD row1    LCD row2   func called on entry rotate      func called on entry switch  func called on exit switch        |  |  |  |  |        +- menue index
// ------------------------------------------------------------------------------------------------------------------------|--|--|--|--|--------|-----------------------------------------------------
 { "        ", "        ", NULL,                            NULL,                        NULL,                             0, 0, 0, 0, 0 }  //  0  null element, if referenced in table then do nothing

// ----- MENUE LEVEL 0 -----
,{ "        ", "        ", (PTR_FUNC)MenuFctER_Operate,     (PTR_FUNC)MenuFctES_Operate, (PTR_FUNC)MenuFctXS_Operate,      0, 0, 2, 2, 2 }  //  1

// ----- MENUE LEVEL 1 -----
,{ "FREQ    ", "        ", NULL,                            NULL,                        NULL,                             6, 3,19,47,51 }  //  2
,{ "WAVEFORM", "        ", NULL,                            NULL,                        NULL,                             2, 4,26,32,42 }  //  3
,{ "O-LEVEL ", "        ", NULL,                            NULL,                        NULL,                             3, 5,12,12,12 }  //  4
,{ "MODE    ", "        ", NULL,                            NULL,                        NULL,                             4, 6, 8, 8, 8 }  //  5
,{ "EXIT    ", "        ", NULL,                            NULL,                        NULL,                             5, 2, 1, 1, 1 }  //  6
,{ "        ", "        ", NULL,                            NULL,                        NULL,                             0, 0, 0, 0, 0 }  //  7  unused

// ----- MENUE LEVEL 2 ----- ( MODE )
,{ "MODE    ", "ANALOG  ", NULL,                            NULL,                        (PTR_FUNC)MenuFctXS_Analog,      10, 9, 1, 1, 1 }  //  8
,{ "MODE    ", "DDS     ", NULL,                            NULL,                        (PTR_FUNC)MenuFctXS_DDS,          8,10, 1, 1, 1 }  //  9
,{ "MODE    ", "DUAL    ", NULL,                            NULL,                        (PTR_FUNC)MenuFctXS_Dual,         9, 8, 1, 1, 1 }  // 10
,{ "        ", "        ", NULL,                            NULL,                        NULL,                             0, 0, 0, 0, 0 }  // 11  unused

// ----- MENUE LEVEL 2 ----- ( O-LEVEL )
,{ "O-LEVEL ", "100%    ", (PTR_FUNC)MenuFctERXS_LVL100,    NULL,                        (PTR_FUNC)MenuFctERXS_LVL100,    15,13, 1, 1, 1 }  // 12
,{ "O-LEVEL ", "50%     ", (PTR_FUNC)MenuFctERXS_LVL50,     NULL,                        (PTR_FUNC)MenuFctERXS_LVL50,     12,14, 1, 1, 1 }  // 13
,{ "O-LEVEL ", "10%     ", (PTR_FUNC)MenuFctERXS_LVL10,     NULL,                        (PTR_FUNC)MenuFctERXS_LVL10,     13,15, 1, 1, 1 }  // 14
,{ "O-LEVEL ", "OFF     ", (PTR_FUNC)MenuFctERXS_LVLOFF,    NULL,                        (PTR_FUNC)MenuFctERXS_LVLOFF,    14,12, 1, 1, 1 }  // 15
,{ "        ", "        ", NULL,                            NULL,                        NULL,                             0, 0, 0, 0, 0 }  // 16  unused

// ----- MENUE LEVEL 2/3 ----- ( FREQ MODE_ANALOG, MODE_DUAL )
,{ "MAX FREQ", "100Hz   ", (PTR_FUNC)MenuFctERXS_A100HZ,    NULL,                        (PTR_FUNC)MenuFctERXS_A100HZ,    24,18, 1, 0, 1 }  // 17
,{ "MAX FREQ", "200Hz   ", (PTR_FUNC)MenuFctERXS_A200HZ,    NULL,                        (PTR_FUNC)MenuFctERXS_A200HZ,    17,19, 1, 0, 1 }  // 18
,{ "MAX FREQ", "1Khz    ", (PTR_FUNC)MenuFctERXS_A1KHZ,     NULL,                        (PTR_FUNC)MenuFctERXS_A1KHZ,     18,20, 1, 0, 1 }  // 19
,{ "MAX FREQ", "2Khz    ", (PTR_FUNC)MenuFctERXS_A2KHZ,     NULL,                        (PTR_FUNC)MenuFctERXS_A2KHZ,     19,21, 1, 0, 1 }  // 20
,{ "MAX FREQ", "10Khz   ", (PTR_FUNC)MenuFctERXS_A10KHZ,    NULL,                        (PTR_FUNC)MenuFctERXS_A10KHZ,    20,22, 1, 0, 1 }  // 21
,{ "MAX FREQ", "20Khz   ", (PTR_FUNC)MenuFctERXS_A20KHZ,    NULL,                        (PTR_FUNC)MenuFctERXS_A20KHZ,    21,23, 1, 0, 1 }  // 22
,{ "MAX FREQ", "100Khz  ", (PTR_FUNC)MenuFctERXS_A100KHZ,   NULL,                        (PTR_FUNC)MenuFctERXS_A100KHZ,   22,24, 1, 0, 1 }  // 23
,{ "MAX FREQ", "200Khz  ", (PTR_FUNC)MenuFctERXS_A200KHZ,   NULL,                        (PTR_FUNC)MenuFctERXS_A200KHZ,   23,17, 1, 0, 1 }  // 24
,{ "        ", "        ", NULL,                            NULL,                        NULL,                             0, 0, 0, 0, 0 }  // 25  unused

// ----- MENUE LEVEL 2/3 ----- ( WAVEFORM MODE_ANALOG, MODE_DUAL )
,{ "WAVE ANA", "SINE    ", (PTR_FUNC)MenuFctERXS_ASine,     NULL,                        (PTR_FUNC)MenuFctERXS_ASine,     30,27, 1, 0, 1 }  // 26
,{ "WAVE ANA", "SQUARE  ", (PTR_FUNC)MenuFctERXS_ASquare,   NULL,                        (PTR_FUNC)MenuFctERXS_ASquare,   26,28, 1, 0, 1 }  // 27
,{ "WAVE ANA", "TRIANGLE", (PTR_FUNC)MenuFctERXS_ATriangle, NULL,                        (PTR_FUNC)MenuFctERXS_ATriangle, 27,29, 1, 0, 1 }  // 28
,{ "WAVE ANA", "PULS    ", (PTR_FUNC)MenuFctERXS_APuls,     NULL,                        (PTR_FUNC)MenuFctERXS_APuls,     28,30, 1, 0, 1 }  // 29
,{ "WAVE ANA", "SAWTOOTH", (PTR_FUNC)MenuFctERXS_ASawtooth, NULL,                        (PTR_FUNC)MenuFctERXS_ASawtooth, 29,26, 1, 0, 1 }  // 30
,{ "        ", "        ", NULL,                            NULL,                        NULL,                             0, 0, 0, 0, 0 }  // 31  unused

// ----- MENUE LEVEL 2/3 ----- ( WAVEFORM MODE_DDS, MODE_DUAL )
,{ "WAVE DDS", "SINE    ", (PTR_FUNC)MenuFctERXS_DSine,     NULL,                        (PTR_FUNC)MenuFctERXS_DSine,     38,33, 0, 1, 1 }  // 32
,{ "WAVE DDS", "SQUARE  ", (PTR_FUNC)MenuFctERXS_DSquare,   NULL,                        (PTR_FUNC)MenuFctERXS_DSquare,   32,34, 0, 1, 1 }  // 33
,{ "WAVE DDS", "TRIANGLE", (PTR_FUNC)MenuFctERXS_DTriangle, NULL,                        (PTR_FUNC)MenuFctERXS_DTriangle, 33,35, 0, 1, 1 }  // 34
,{ "WAVE DDS", "SAWTOOTH", (PTR_FUNC)MenuFctERXS_DSawtooth, NULL,                        (PTR_FUNC)MenuFctERXS_DSawtooth, 34,36, 0, 1, 1 }  // 35
,{ "WAVE DDS", "PULS    ", (PTR_FUNC)MenuFctERXS_DPuls,     NULL,                        (PTR_FUNC)MenuFctERXS_DPuls,     35,37, 0, 1, 1 }  // 36
,{ "WAVE DDS", "STEP    ", (PTR_FUNC)MenuFctERXS_DStep,     NULL,                        (PTR_FUNC)MenuFctERXS_DStep,     36,38, 0, 1, 1 }  // 37
,{ "WAVE DDS", "RAMP    ", (PTR_FUNC)MenuFctERXS_DRamp,     NULL,                        (PTR_FUNC)MenuFctERXS_DRamp,     37,32, 0, 1, 1 }  // 38
,{ "        ", "        ", NULL,                            NULL,                        NULL,                             0, 0, 0, 0, 0 }  // 39  unused
,{ "        ", "        ", NULL,                            NULL,                        NULL,                             0, 0, 0, 0, 0 }  // 40  unused
,{ "        ", "        ", NULL,                            NULL,                        NULL,                             0, 0, 0, 0, 0 }  // 41  unused

// ----- MENUE LEVEL 2 ----- ( WAVEFORM MODE_DUAL )
,{ "SELECT  ", "ANALOG  ", NULL,                            NULL,                        NULL,                            43,43, 0, 0,26 }  // 42
,{ "SELECT  ", "DDS     ", NULL,                            NULL,                        NULL,                            42,42, 0, 0,32 }  // 43
,{ "        ", "        ", NULL,                            NULL,                        NULL,                             0, 0, 0, 0, 0 }  // 44  unused

// ----- MENUE LEVEL 2/3 ----- ( FREQ MODE_DDS, MODE_DUAL )
,{ "F STEP  ", "1x0000  ", NULL,                            NULL,                        (PTR_FUNC)MenuFctXS_STEP10000,   49,46, 0, 1, 1 }  // 45
,{ "F STEP  ", "10x000  ", NULL,                            NULL,                        (PTR_FUNC)MenuFctXS_STEP1000,    45,47, 0, 1, 1 }  // 46
,{ "F STEP  ", "100x00  ", NULL,                            NULL,                        (PTR_FUNC)MenuFctXS_STEP100,     46,48, 0, 1, 1 }  // 47
,{ "F STEP  ", "1000x0  ", NULL,                            NULL,                        (PTR_FUNC)MenuFctXS_STEP10,      47,49, 0, 1, 1 }  // 48
,{ "F STEP  ", "10000x  ", NULL,                            NULL,                        (PTR_FUNC)MenuFctXS_STEP1,       48,45, 0, 1, 1 }  // 49
,{ "        ", "        ", NULL,                            NULL,                        NULL,                             0, 0, 0, 0, 0 }  // 50  unused

// ----- MENUE LEVEL 2 ----- ( FREQ MODE_DUAL )
,{ "SELECT  ", "ANALOG  ", NULL,                            NULL,                        NULL,                            52,52, 0, 0,19 }  // 51
,{ "SELECT  ", "DDS     ", NULL,                            NULL,                        NULL,                            51,51, 0, 0,47 }  // 52
,{ "        ", "        ", NULL,                            NULL,                        NULL,                             0, 0, 0, 0, 0 }  // 53  unused
};

// + + + MACRO DEFINITIONS + + +

#define ROTLEFT   rotationDirection == ROT_LEFT
#define ROTRIGHT  rotationDirection == ROT_RIGHT

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// ROUTINE     :  main()
// DESCRIPTION :  Program main entry
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
int main()
{
  // + + + INITIALIZATION + + +

  cli();

  InitAnalogFGen();
  SetFGenProperties();

  InitDDSFGen( ddsWaveForm, ddsFrequency );


#if( _DEBUG )
  InitUSART0();
#endif
  Debug( "CLEAR" );
  Debug( " + + + Function Generator Debug Console + + + " );

  Debug( " ... Init Encoder and register callback functions" );

  InitEncoder( (_PTR_FUNC_)CallBack_SwitchPressed
             , (_PTR_FUNC_)CallBack_EncLeftRotate
             , (_PTR_FUNC_)CallBack_EncRightRotate );

  Debug( " ... Init Display" );
  Debug( " ... FuncGen Vers 3.0" );

  InitLCD();
  LCD( "FuncGen ", "Vers 1.0" );
  Delay_s( 1 );

  MenuFctES_Operate();                                                              // initializes the LCD for operational mode

  // initialization done, free interrupts

  sei();

  // + + + INFINITE OPERATIONAL LOOP + + +

  while(1) {
    switch ( runMode ) {

      case MODE_ANALOG:
        // nothing to do
        break;

      case MODE_DDS:
        Debug( " ... DDS Generator started (MODE_DDS)" );
        RunDDSFGen();
        Debug( " ... DDS Generator stopped (MODE_DDS)" );
        break;

      case MODE_DUAL:
        Debug( " ... DDS Generator started (MODE_DUAL)" );
        RunDDSFGen();
        Debug( " ... DDS Generator stopped (MODE_DUAL)" );
        break;

      default:
        break;
    }
  } // endless
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// ROUTINE     :  ActualizeLCD()
// DESCRIPTION :  Actualize the LCD display
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void ActualizeLCD()
{
  Debug( " ... ActualizeLCD" );
  LCD( array_Menue[menueIndex].txtRow1, array_Menue[menueIndex].txtRow2 );
  return;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// ROUTINE     :  SetFGenProperties()
// DESCRIPTION :  Depending on runMode set the function generator properties.
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void SetFGenProperties()
{
  Debug( " ... SetFGenProperties" );

  switch( runMode ) {

    case MODE_ANALOG:
      SetOutputLevel( LVLOFF );
      StopDDSFGen();
      SetAnalogMaxF( analogMaxFrequency );
      SetWaveSource( analogWaveForm );
      SetOutputLevel( outputLevel );
      break;

    case MODE_DDS:
      SetOutputLevel( LVLOFF );
      StopDDSFGen();
      InitDDSFGen( ddsWaveForm, ddsFrequency );
      SetWaveSource( DDS );
      SetOutputLevel( outputLevel );
      break;

    case MODE_DUAL:
      SetOutputLevel( LVLOFF );
      StopDDSFGen();
      SetAnalogMaxF( analogMaxFrequency );
      SetWaveSource( analogWaveForm );
      InitDDSFGen( ddsWaveForm, ddsFrequency );
      SetOutputLevel( outputLevel );
      break;

    default:
      Debug( " ERROR in SetFGenProperties " );
      break;
  }

  return;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// ROUTINE     :  CallBack_SwitchPressed()
// DESCRIPTION :  Incremental encoder switch callback routine
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void CallBack_SwitchPressed()
{
  uint8_t switchIndex;

  Debug( " ... CallBack_SwitchPressed" );

  switch( runMode ) {

    case MODE_ANALOG:
      switchIndex = array_Menue[menueIndex].switchIndexModeAnalog;
      break;

    case MODE_DDS:
      switchIndex = array_Menue[menueIndex].switchIndexModeDDS;
      break;

    case MODE_DUAL:
      switchIndex = array_Menue[menueIndex].switchIndexModeDual;
      break;

    default:
      Debug( " ERROR in CallBack_SwitchPresse " );
      break;
  }

  if( switchIndex != 0 ) {                                                          // do nothing if null element is addressed

    if( array_Menue[menueIndex].funcCallOnExitSelect != NULL ) {                    // if there is a function assigned to the new index ..
      array_Menue[menueIndex].funcCallOnExitSelect();                               // .. call it
    }

    menueIndex = switchIndex;                                                       // position to next menue item in tree ..
    ActualizeLCD();                                                                 // .. and actualize LCD

    if( array_Menue[menueIndex].funcCallOnEntrySelect != NULL ) {                   // if there is a function assigned to the new index ..
      array_Menue[menueIndex].funcCallOnEntrySelect();                              // .. call it
    }
  }

  return;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// ROUTINE     :  CallBack_EncLeftRotate()
// DESCRIPTION :  Incremental encoder callback routine, left rotate
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void CallBack_EncLeftRotate()
{
  Debug( " ... CallBack_EncLeftRotate" );

  rotationDirection = ROT_LEFT;

  if( array_Menue[menueIndex].prevIndex != 0 ) {                                    // do nothing if null element is addressed

    menueIndex = array_Menue[menueIndex].prevIndex;                                 // position to next menue item in tree ..
    ActualizeLCD();                                                                 // .. and actualize LCD
  }

  if( array_Menue[menueIndex].funcCallOnEntryRotate != NULL ) {                     // if there is a function assigned to the new index ..
    array_Menue[menueIndex].funcCallOnEntryRotate();                                // .. call it
  }

  return;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// ROUTINE     :  CallBack_EncRightRotate()
// DESCRIPTION :  Incremental encoder callback routine, right rotate
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void CallBack_EncRightRotate()
{
  Debug( " ... CallBack_EncRightRotate" );

  rotationDirection = ROT_RIGHT;

  if( array_Menue[menueIndex].nextIndex != 0 ) {                                    // do nothing if null element addressed

    menueIndex = array_Menue[menueIndex].nextIndex;                                 // position to next menue item in tree
    ActualizeLCD();                                                                 // and actualize LCD

  }

  if( array_Menue[menueIndex].funcCallOnEntryRotate != NULL ) {                     // if there is a function assigned to the new index ..
    array_Menue[menueIndex].funcCallOnEntryRotate();                                // .. call it
  }

  return;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// ROUTINE     :  MenuFctER_Operate()
// DESCRIPTION :  In case EncAB is not locked in opertional mode this routine
//                is called.
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void MenuFctER_Operate()
{
  char szFrequency[8] = "--------";

  Debug( " ... MenuFctER_Operate" );

  // serve encoder rotate 
  if( runMode == MODE_DDS || runMode == MODE_DUAL) {                                // just to be safe, the EncAB switches should be usually locked if not in mode DDS or DUAL

    if( ROTLEFT ) {
      if( ddsFrequency < ddsFrequencyStep ) {
        ddsFrequency = ddsFrequencyMin; 
      } else {
        ddsFrequency -= ddsFrequencyStep;
      }
    } else if( ROTRIGHT ) {
      ddsFrequency += ddsFrequencyStep;
      if( ddsFrequency > ddsFrequencyMax ) {
        ddsFrequency = ddsFrequencyMax;
      }
    }
    SetFGenProperties();

    snprintf( szFrequency, 8, "%lu", ddsFrequency );                                // actualize LCD frequency display
    LCD( "", "        " );
    LCD( "", szFrequency );
  }
  return;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// ROUTINE     :  MenuFctES_Operate()
// DESCRIPTION :  Actualizes the LCD when generator goes into operational mode
//                and lock incremental encode for rotate
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void MenuFctES_Operate()
{

  Debug( " ... MenuFctES_Operate" );

  // menue text constants in operational mode
  static char* txtMode[] =
               { "     ANA"                                                         // corresponds with ENUM_RUN_MODE
               , "     DDS" };

  static char* txtFrequency[] =
               { "        "                                                         // corresponds with ENUM_MAX_FREQUENCY
               , "100H"
               , "200H"
               , "1K  "
               , "2K  "
               , "10K "
               , "20K "
               , "100K"
               , "200K" };

  static char* txtLevel[] =
               { "        "                                                         // corresponds with ENUM_OUTPUT_LEVEL
               , "    OFF "
               , "    10% "
               , "    50% "
               , "    100%" };

  static char* txtAnalogWaveForm[] = 
               { "        "                                                         // corresponds with ENUM_ANALOG_SHAPE
               , "Sine"
               , "Squa"
               , "Tri"
               , "Puls"
               , "Saw" };

  static char* txtDDSWaveForm[] = 
               { "Sine"                                                             // corresponds with ENUM_DDS_SHAPE
               , "Squa"
               , "Tri"
               , "Saw" 
               , "Puls" 
               , "Step"
               , "Ramp" };

  static char* txtAnalogWaveFormDual[] = 
               { "        "                                                         // corresponds with ENUM_ANALOG_SHAPE
               , "Sin"
               , "Sqr"
               , "Tri"
               , "Pls"
               , "Saw" };

  static char* txtDDSWaveFormDual[] = 
               { "    Sin "                                                          // corresponds with ENUM_DDS_SHAPE
               , "    Sqr "
               , "    Tri "
               , "    Saw "
               , "    Pls "
               , "    Stp "
               , "    Rmp " };

  char szFrequency[8] = "--------";

  switch( runMode ) {

    case MODE_ANALOG:

      //  LCD example:
      //    +------ wave form
      //    |   +-- analog mode
      //    |   |
      // +--------+
      // |Sine ANA|
      // |1K  100%|
      // +--------+
      //   |   |
      //   |   +--- output level
      //   +------- max frequency

      // LCD rows are overlaid
      LCD( txtMode[runMode], txtLevel[outputLevel] );
      LCD( txtAnalogWaveForm[analogWaveForm], txtFrequency[analogMaxFrequency] );
      LockEncAB();
      break;

    case MODE_DDS:

      //  LCD example:
      //    +------ wave form
      //    |   +-- DDS mode
      //    |   | 
      // +--------+
      // |Sine DDS|
      // |1000    |
      // +--------+
      //    |
      //    +------ frequency

      snprintf( szFrequency, 8, "%lu", ddsFrequency ); 
      LCD( "        ", "        " );                                                // clear LCD
      // LCD rows are overlaid
      LCD( txtMode[runMode], szFrequency );
      LCD( txtDDSWaveForm[ddsWaveForm], "" );
      // do not lock EncAB to allow frequency adjustment
      break;

    case MODE_DUAL:

      //  LCD example:
      //    +------ analog wave form
      //    |   +-- DDS wave form
      //    |   | 
      // +--------+
      // |Sin Sin |
      // |1000    |
      // +--------+
      //    |
      //    +------ DDS frequency

      snprintf( szFrequency, 8, "%lu", ddsFrequency ); 
      LCD( "        ", "        " );                                                // clear LCD
      // LCD rows are overlaid
      LCD( txtDDSWaveFormDual[ddsWaveForm], szFrequency );
      LCD( txtAnalogWaveFormDual[analogWaveForm], "" );
      // do not lock EncAB to allow frequency adjustment
      break;

    default:
      Debug( " ERROR in MenuFctES_Operate " );
      break;
  }

  return;
};

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// ROUTINE     :  MenuFctXS_Operate()
// DESCRIPTION :  Release incremental encoder for rotate when leaving 
//                operational mode
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void MenuFctXS_Operate()
{
  Debug( " ... MenuFctES_Operate" );
  ReleaseEncAB();
  return;
};

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// ROUTINE     :  MenuFctXS_Analog()
// DESCRIPTION :  Set generator in analog mode
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void MenuFctXS_Analog()
{
  Debug( " ... MenuFctXS_Analog" );
  runMode = MODE_ANALOG;
  SetFGenProperties();
  return;
};

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// ROUTINE     :  MenuFctXS_DDS()
// DESCRIPTION :  Set generator in DDS mode
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void MenuFctXS_DDS()
{
  Debug( " ... MenuFctXS_DDS" );
  runMode = MODE_DDS;
  SetFGenProperties();
  return;
};

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// ROUTINE     :  MenuFctXS_Dual()
// DESCRIPTION :  Set generator in Analog + DDS mode
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void MenuFctXS_Dual()
{
  Debug( " ... MenuFctXS_Dual" );
  runMode = MODE_DUAL;
  SetFGenProperties();
  return;
};

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// ROUTINE     :  MenuFctERXS_ASine()
// DESCRIPTION :  Set analog generator wave form sine
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void MenuFctERXS_ASine()
{
  Debug( " ... MenuFctERXS_ASine" );
  analogWaveForm = ANALOG_SINE;
  SetFGenProperties();
  return;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// ROUTINE     :  MenuFctERXS_ASquare()
// DESCRIPTION :  Set analog generator wave form square
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void MenuFctERXS_ASquare()
{
  Debug( " ... MenuFctERXS_ASquare" );
  analogWaveForm = ANALOG_SQUARE;
  SetFGenProperties();
  return;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// ROUTINE     :  MenuFctERXS_ATriangle()
// DESCRIPTION :  Set analog generator wave form triangle
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void MenuFctERXS_ATriangle()
{
  Debug( " ... MenuFctERXS_ATriangle" );
  analogWaveForm = ANALOG_TRIANGLE;
  SetFGenProperties();
  return;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// ROUTINE     :  MenuFctERXS_APuls()
// DESCRIPTION :  Set analog generator wave form puls
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void MenuFctERXS_APuls()
{
  Debug( " ... MenuFctERXS_APuls" );
  analogWaveForm = ANALOG_PULS;
  SetFGenProperties();
  return;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// ROUTINE     :  MenuFctERXS_ASawtooth()
// DESCRIPTION :  Set analog generator wave form sawtooth
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void MenuFctERXS_ASawtooth()
{
  Debug( " ... MenuFctERXS_ASawtooth" );
  analogWaveForm = ANALOG_SAWTOOTH;
  SetFGenProperties();
  return;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// ROUTINE     :  MenuFctERXS_DSine()
// DESCRIPTION :  Set DDS generator wave form sine
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void MenuFctERXS_DSine()
{
  Debug( " ... enuFctERXS_DSine" );
  ddsWaveForm = DDS_SINE;
  SetFGenProperties();
  return;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// ROUTINE     :  MenuFctERXS_DSquare()
// DESCRIPTION :  Set DDS generator wave form square
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void MenuFctERXS_DSquare()
{
  Debug( " ... MenuFctERXS_DSquare" );
  ddsWaveForm = DDS_SQUARE;
  SetFGenProperties();
  return;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// ROUTINE     :  MenuFctERXS_DTriangle()
// DESCRIPTION :  Set DDS generator wave form triangle
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void MenuFctERXS_DTriangle()
{
  Debug( " ... MenuFctERXS_DTriangle" );
  ddsWaveForm = DDS_TRIANGLE;
  SetFGenProperties();
  return;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// ROUTINE     :  MenuFctERXS_DSawtooth()
// DESCRIPTION :  Set DDS generator wave form sawtooth
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void MenuFctERXS_DSawtooth()
{
  Debug( " ... MenuFctERXS_DSawtooth" );
  ddsWaveForm = DDS_SAWTOOTH;
  SetFGenProperties();
  return;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// ROUTINE     :  MenuFctERXS_DPuls()
// DESCRIPTION :  Set DDS generator wave form puls
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void MenuFctERXS_DPuls()
{
  Debug( " ... MenuFctERXS_DPuls" );
  ddsWaveForm = DDS_PULS;
  SetFGenProperties();
  return;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// ROUTINE     :  MenuFctERXS_DStep()
// DESCRIPTION :  Set DDS generator wave form step
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void MenuFctERXS_DStep()
{
  Debug( " ... MenuFctERXS_DStep" );
  ddsWaveForm = DDS_STEP;
  SetFGenProperties();
  return;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// ROUTINE     :  MenuFctERXS_DRamp()
// DESCRIPTION :  Set DDS generator wave form ramp
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void MenuFctERXS_DRamp()
{
  Debug( " ... MenuFctERXS_DRamp" );
  ddsWaveForm = DDS_RAMP;
  SetFGenProperties();
  return;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// ROUTINE     :  MenuFctERXS_A100HZ()
// DESCRIPTION :  Set analog generator max frequency
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void MenuFctERXS_A100HZ()
{
  Debug( " ... MenuFctERXS_A100HZ" );
  analogMaxFrequency = F100HZ;
  SetFGenProperties();
  return;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// ROUTINE     :  MenuFctERXS_A200HZ()
// DESCRIPTION :  Set analog generator max frequency
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void MenuFctERXS_A200HZ()
{
  Debug( " ... MenuFctERXS_A200HZ" );
  analogMaxFrequency = F200HZ;
  SetFGenProperties();
  return;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// ROUTINE     :  MenuFctERXS_A1KHZ()
// DESCRIPTION :  Set analog generator max frequency
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void MenuFctERXS_A1KHZ()
{
  Debug( " ... MenuFctERXS_A1KHZ" );
  analogMaxFrequency = F1000HZ;
  SetFGenProperties();
  return;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// ROUTINE     :  MenuFctERXS_A2KHZ()
// DESCRIPTION :  Set analog generator max frequency
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void MenuFctERXS_A2KHZ()
{
  Debug( " ... MenuFctERXS_A2KHZ" );
  analogMaxFrequency = F2000HZ;
  SetFGenProperties();
  return;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// ROUTINE     :  MenuFctERXS_A10KHZ()
// DESCRIPTION :  Set analog generator max frequency
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void MenuFctERXS_A10KHZ()
{
  Debug( " ... MenuFctERXS_A10KHZ" );
  analogMaxFrequency = F10000HZ;
  SetFGenProperties();
  return;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// ROUTINE     :  MenuFctERXS_A20KHZ()
// DESCRIPTION :  Set analog generator max frequency
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void MenuFctERXS_A20KHZ()
{
  Debug( " ... MenuFctERXS_A20KHZ" );
  analogMaxFrequency = F20000HZ;
  SetFGenProperties();
  return;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// ROUTINE     :  MenuFctERXS_A100KHZ()
// DESCRIPTION :  Set analog generator max frequency
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void MenuFctERXS_A100KHZ()
{
  Debug( " ... MenuFctERXS_A100KHZ" );
  analogMaxFrequency = F100000HZ;
  SetFGenProperties();
  return;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// ROUTINE     :  MenuFctERXS_A200KHZ()
// DESCRIPTION :  Set analog generator max frequency
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void MenuFctERXS_A200KHZ()
{
  Debug( " ... MenuFctERXS_A200KHZ" );
  analogMaxFrequency = F200000HZ;
  SetFGenProperties();
  return;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// ROUTINE     :  MenuFctERXS_LVL100()
// DESCRIPTION :  Set generator max output level to 100%
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void MenuFctERXS_LVL100()
{
  Debug( " ... MenuFctERXS_LVL100" );
  outputLevel = LVL100;
  SetFGenProperties();
  return;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// ROUTINE     :  MenuFctERXS_LVL50()
// DESCRIPTION :  Set generator max output level to 50%
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void MenuFctERXS_LVL50()
{
  Debug( " ... MenuFctERXS_LVL50" );
  outputLevel = LVL50;
  SetFGenProperties();
  return;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// ROUTINE     :  MenuFctERXS_LVL10()
// DESCRIPTION :  Set generator max output level to 10%
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void MenuFctERXS_LVL10()
{
  Debug( " ... MenuFctERXS_LVL10" );
  outputLevel = LVL10;
  SetFGenProperties();
  return;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// ROUTINE     :  MenuFctERXS_LVLOFF()
// DESCRIPTION :  Set generator output to off
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void MenuFctERXS_LVLOFF()
{
  Debug( " ... MenuFctERXS_LVLOFF" );
  outputLevel = LVLOFF;
  SetFGenProperties();
  return;
}
// * *

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// ROUTINE     :  MenuFctXS_STEP1()
// DESCRIPTION :  Set DDS frequency adjustment resolution to 1Hz
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void MenuFctXS_STEP1()
{
  Debug( " ... MenuFctXS_STEP1" );
  ddsFrequencyStep = 1;
  return;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// ROUTINE     :  MenuFctXS_STEP10()
// DESCRIPTION :  Set DDS frequency adjustment resolution to 1Hz
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void MenuFctXS_STEP10()
{
  Debug( " ... MenuFctXS_STEP10" );
  ddsFrequencyStep = 10;
  return;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// ROUTINE     :  MenuFctXS_STEP100()
// DESCRIPTION :  Set DDS frequency adjustment resolution to 100Hz
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void MenuFctXS_STEP100()
{
  Debug( " ... MenuFctXS_STEP100" );
  ddsFrequencyStep = 100;
  return;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// ROUTINE     :  MenuFctXS_STEP1000()
// DESCRIPTION :  Set DDS frequency adjustment resolution to 1000Hz
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void MenuFctXS_STEP1000()
{
  Debug( " ... MenuFctXS_STEP1000" );
  ddsFrequencyStep = 1000;
  return;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// ROUTINE     :  MenuFctXS_STEP10000()
// DESCRIPTION :  Set DDS frequency adjustment resolution to 10000Hz
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void MenuFctXS_STEP10000()
{
  Debug( " ... MenuFctXS_STEP10000" );
  ddsFrequencyStep = 10000;
  return;
}

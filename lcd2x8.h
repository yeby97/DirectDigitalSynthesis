#ifndef DISPLAY_H_
#define DISPLAY_H_
// + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
// + AUTHOR      : Guido Trensch, 2012
// + FILE        : display2x8.h
// + DESCRIPTION : display2x8.asm C-interface wrapper
// +
// +               Provides following functions:
// +
// +               InitDisplay ( none          )
// +               Display     ( ENUM_ROW row, char* pText )
// + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +

//
// F U N C T I O N S
//

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// WRAPPER     :  InitLCD()
// ROUTINE     :  ASM_InitLCD
// DESCRIPTION :  Initialize the 2 x 8 LCD display
// PARAMETERS  :  none
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
extern void* ASM_InitLCD();                      // prototype
#define InitLCD ASM_InitLCD                      // function wrapper

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// WRAPPER     :  LCD( char* pText_row1, char* pText_row2 )
// ROUTINE     :  ASM_LCD
// DESCRIPTION :  Displays text on the LCD
// PARAMETERS  :  pText_row1 ... pointer to C-string with max length of 8 to 
//                               be displayed in row 1 of the LCD 
//                pText_row2 ... pointer to C-string with max length of 8
//                               be displayed in row 2 of the LCD 
//                Note: If a NULL pointer or "" is handed over, the row
//                      will not be modified.
//                      A row can be deleted by specifiying "        ".
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
extern void* ASM_LCD();                          // prototype

#define skip_row    0x7F                         // largest unsigned value
#define charsPerRow 8

volatile char*    glob_pText_row1 = NULL;        // global variable, pointer to string
volatile char*    glob_pText_row2 = NULL;        // global variable, pointer to string
volatile uint8_t  glob_len_row1   = skip_row;    // global variable, len string
volatile uint8_t  glob_len_row2   = skip_row;    // global variable, len string
                                                 // function wrapper
#define LCD( X, Y )                            \
{                                              \
  glob_pText_row1 = X;                         \
  if( NULL != glob_pText_row1 ) {              \
    glob_len_row1 = strnlen( X, charsPerRow ); \
    if( 0 == glob_len_row1 ) {                 \
       glob_len_row1 = skip_row;               \
    }                                          \
  }                                            \
  glob_pText_row2 = Y;                         \
  if( NULL != glob_pText_row2 ) {              \
    glob_len_row2 = strnlen( Y, charsPerRow ); \
    if( 0 == glob_len_row2 ) {                 \
       glob_len_row2 = skip_row;               \
    }                                          \
  }                                            \
  ASM_LCD();                                   \
}

#endif  // DISPLAY_H_

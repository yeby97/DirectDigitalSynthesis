; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
; + AUTHOR      : Guido Trensch, 2012
; + FILE        : display2x8.asm
; + DESCRIPTION : LCD Display driver
; +
; +               Implements following functions:
; +
; +               ASM_InitLCD
; +               ASM_Display
; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
                    #include   "equates1284P.inc"

; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
; +                  L C D   D I S P L A Y   P O R T   S E T T I N G                                              +
; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
                     .EQU       LCDPort,      PORTB
                     .EQU       LCDDDReg,     DDRB
                     .EQU       CmdData,      PB2                 ; RS    display function select, 0 = command, 1 = data
                     .EQU       Enable,       PB3                 ; EN    display enable           1 = enable
                     .EQU       DATA4,        PB4                 ; DB4   display data 4
                     .EQU       DATA5,        PB5                 ; DB5   display data 5
                     .EQU       DATA6,        PB6                 ; DB6   display data 6
                     .EQU       DATA7,        PB7                 ; DB7   display data 7
                                                                  ; Note: R/W always 0 - write only, requires no port pin
                     .EQU       CharsPerRow,  0x08

; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
; +                  E X T E R N A L   F U N C T I O N S                                                          +
; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
; +                  ASM_InitLCD                                                                                  +
; +                                                                                                               +
; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
                    .GLOBAL     ASM_InitLCD
                    .FUNC       ASM_InitLCD
ASM_InitLCD:         PUSH       R16                               ; save register

                     IN         R16,SREG                          ; keep interrupt flag ..
                     PUSH       R16                               ; .. on stack
                     CLI                                          ; disable interrupts

;                    + + + WAIT AFTER POWER UP + + +

                     RCall      Delay_5ms
                     RCall      Delay_5ms
                     RCall      Delay_5ms
                     RCall      Delay_5ms
                     RCall      Delay_5ms
                     RCall      Delay_5ms
                     RCall      Delay_5ms
                     RCall      Delay_5ms

;                    + + + INITIALIZE LCD PORT + + +

                     IN         R16,LCDDDReg                      ; read current configuration
                     ORI        R16,(1 << CmdData) | (1 << Enable) | (1 << DATA7) | (1 << DATA6) | (1 << DATA5) | (1 << DATA4)
                     OUT        LCDDDReg,R16                      ; configure lines for output
                     CLR        R16                               ; R16:= x'00'
                     OUT        LCDPort,R16                       ; all lines 0

;                    + + + INITIALIZE LCD + + +

                     LDI        R16,0b00110000                    ; D7 D6 D5 D4    needs to be sent 3 times with delay in between
                     OUT        LCDPort,R16                       ; 0  0  1  1
                     RCALL      LCD_Enable                        ; enable LCD           1.
                     RCall      Delay_5ms                         ; wait
                     RCALL      LCD_Enable                        ; enable LCD           2.
                     RCall      Delay_5ms                         ; wait
                     RCALL      LCD_Enable                        ; enable LCD           3.
                     RCall      Delay_5ms                         ; wait

                                                                  ; set 4 bit mode  
                     LDI        R16,0b00100000                    ; D7 D6 D5 D4
                     OUT        LCDPort,R16                       ; 0  0  1  0            4 bit mode
                     RCALL      LCD_Enable                        ; enable LCD
                     RCall      Delay_5ms                         ; wait

                     RCALL      LCD_Function_Set
                     RCALL      LCD_Display_On
                     RCALL      LCD_Entry_Mode_Set
                     RCALL      LCD_Clear

                     LDI        R16,'I'
                     RCALL      LCD_Data
                     LDI        R16,'n'
                     RCALL      LCD_Data
                     LDI        R16,'i'
                     RCALL      LCD_Data
                     LDI        R16,'t'
                     RCALL      LCD_Data
                     LDI        R16,' '
                     RCALL      LCD_Data
                     LDI        R16,'L'
                     RCALL      LCD_Data
                     LDI        R16,'C'
                     RCALL      LCD_Data
                     LDI        R16,'D'
                     RCALL      LCD_Data

                     RCALL      LCD_SecondRow

                     LDI        R16,'F'
                     RCALL      LCD_Data
                     LDI        R16,'u'
                     RCALL      LCD_Data
                     LDI        R16,'n'
                     RCALL      LCD_Data
                     LDI        R16,'c'
                     RCALL      LCD_Data
                     LDI        R16,' '
                     RCALL      LCD_Data
                     LDI        R16,'G'
                     RCALL      LCD_Data
                     LDI        R16,'e'
                     RCALL      LCD_Data
                     LDI        R16,'n'
                     RCALL      LCD_Data

                     POP        R16                               ; restore interrupt flag from stack
                     OUT        SREG,R16

                     POP        R16                               ; restore register
                     RET
                    .ENDFUNC

; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
; +                  ASM_LCD                                                                                      +
; +                                                                                                               +
; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
                    .EXTERN     glob_pText_row1                   ; global variable, pointer to string
                    .EXTERN     glob_len_row1                     ; global variable, length string (max 8)
                    .EXTERN     glob_pText_row2                   ; global variable, pointer to string
                    .EXTERN     glob_len_row2                     ; global variable, length string (max 8)
                    .GLOBAL     ASM_LCD
                    .FUNC       ASM_LCD
ASM_LCD:             PUSH       R16                               ; save registers
                     PUSH       R17
                     PUSH       R26
                     PUSH       R27

                     IN         R16,SREG                          ; keep interrupt flag ..
                     PUSH       R16                               ; .. on stack
                     CLI                                          ; disable interrupts

                     RCALL      LCD_Home

                     LDS        R17,glob_len_row1                 ; R17:= len row1
                     CPI        R17,CharsPerRow + 1               ; len >= 0x09 ?
                     BRGE       LCD_SkipRow1                      ; -->> skip row1

                     LDS        R26,glob_pText_row1               ; X low_address
                     LDS        R27,glob_pText_row1 + 1           ; X high_address
LCD_NextCharRow1:    LD         R16,X+
                     RCALL      LCD_Data
                     DEC        R17
                     BRNE       LCD_NextCharRow1                  ; -->> nexht char in row 1

LCD_SkipRow1:        LDS        R17,glob_len_row2                 ; R17:= len row1
                     CPI        R17,CharsPerRow + 1               ; len >= 0x09 ?
                     BRGE       LCD_SkipRow2                      ; -->> skip row2

                     RCALL      LCD_SecondRow                     ; address row2

                     LDS        R26,glob_pText_row2               ; X low_address
                     LDS        R27,glob_pText_row2 + 1           ; X high_address
LCD_NextCharRow2:    LD         R16,X+
                     RCALL      LCD_Data
                     DEC        R17
                     BRNE       LCD_NextCharRow2                  ; -->> nexht char in row 2

LCD_SkipRow2:        POP        R16                               ; restore interrupt flag from stack
                     OUT        SREG,R16

                     POP        R27
                     POP        R26
                     POP        R17
                     POP        R16
                     RET                                          ; -->> return to caller
                    .ENDFUNC

; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
; +                  I N T E R N A L   S U B R O U T I N E S                                                      +
; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
; +                  LCD_Function_Set                                                                             +
; +                                                                                                               +
; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
LCD_Function_Set:    PUSH       R16
                     LDI        R16,0b00101000                    ; D7 D6 D5 D4 D7 D6 D5 D4    CMD Function Set: 4 bit mode, 2 rows, Font 5x7
                     RCALL      LCD_Command                       ; 0  0  1  0  1  0  0  0 
                                                                  ;             |  |
                                                                  ;             |  +------------------------------- 5x7 (Note: 8x10 does not allow 2 rows)
                                                                  ;             +---------------------------------- 2 rows
                     POP        R16
                     RET

; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
; +                  LCD_Display_On                                                                               +
; +                                                                                                               +
; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
LCD_Display_On:      PUSH       R16
                     LDI        R16,0b00001100                    ; D7 D6 D5 D4 D7 D6 D5 D4    CMD Display On/Off: display on, cursor off, cursor does not blink
                     RCALL      LCD_Command                       ; 0  0  0  0  1  1  0  0 
                                                                  ;                |  |  |
                                                                  ;                |  |  +------------------------- cusor does not blink
                                                                  ;                |  +---------------------------- cursor off
                                                                  ;                +------------------------------- display on
                     POP        R16
                     RET

; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
; +                  LCD_Entry_Mode_Set                                                                           +
; +                                                                                                               +
; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
LCD_Entry_Mode_Set:  PUSH       R16
                     LDI        R16,0b00000110                    ; D7 D6 D5 D4 D7 D6 D5 D4    CMD Entry Mode Set: cursor move direction is increment
                     RCALL      LCD_Command                       ; 0  0  0  0  0  1  1  0
                                                                  ;                   |   
                                                                  ;                   +---------------------------- increment
                     POP        R16
                     RET


; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
; +                  LCD_ShiftLeft                                                                                +
; +                                                                                                               +
; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
LCD_ShiftLeft:       PUSH       R16
                     LDI        R16,0b00011000                    ; D7 D6 D5 D4 D7 D6 D5 D4    CMD Cusrsor and Display Shift: Display Shift left 
                     RCALL      LCD_Command                       ; 0  0  0  1  1  0  0  0
                     POP        R16
                     RET

; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
; +                  LCD_Home                                                                                     +
; +                                                                                                               +
; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
LCD_Home:            PUSH       R16
                     LDI        R16,0b00000010                    ; D7 D6 D5 D4 D7 D6 D5 D4    CMD Return Home
                     RCALL      LCD_Command                       ; 0  0  0  0  0  0  1  0
                     POP        R16
                     RET

; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
; +                  LCD_Clear                                                                                    +
; +                                                                                                               +
; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
LCD_Clear:           PUSH       R16
                     LDI        R16,0b00000001                    ; D7 D6 D5 D4 D7 D6 D5 D4    CMD Clear Display
                     RCALL      LCD_Command                       ; 0  0  0  0  0  0  0  1
                     POP        R16
                     RET

; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
; +                  LCD_SecondRow                                                                                +
; +                                                                                                               +
; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
LCD_SecondRow:       PUSH       R16
                     LDI        R16,0b10101000                    ; D7 D6 D5 D4 D7 D6 D5 D4    CMD Set DD Ram Address
                     RCALL      LCD_Command                       ; 1  0  1  0  1  0  0  0
                     POP        R16
                     RET

; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
; +                  LCD_Enable                                                                                   +
; +                                                                                                               +
; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
LCD_Enable:          SBI        LCDPort,Enable                   ; toggle the enable pin with some delay
                     NOP
                     NOP
                     NOP
                     NOP
                     NOP
                     NOP
                     NOP
                     NOP
                     NOP
                     NOP
                     CBI        LCDPort,Enable
                     RET

; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
; +                  LCD_Command                                                                                  +
; +                  INPUT: R16 .. contains the comand byte                                                       +
; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
LCD_Command:         PUSH       R16
                     PUSH       R17
                     MOV        R17,R16                           ; save R16
                     ANDI       R16,0xF0                          ; clear lower 4 bit, CmdData is off
                     OUT        LCDPort,R16
                     RCALL      LCD_Enable
                     ANDI       R17,0x0F                          ; clear upper 4 bit
                     SWAP       R17                               ; swap nibble, CmdData is off
                     OUT        LCDPort,R17
                     RCALL      LCD_Enable
                     RCALL      Delay_5ms                         ; wait
                     POP        R17
                     POP        R16
                     RET

; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
; +                  LCD_Data                                                                                     +
; +                  INPUT: R16 .. contains the data byte                                                         +
; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
LCD_Data:            PUSH       R16
                     PUSH       R17
                     MOV        R17,R16                           ; save R16
                     ANDI       R16,0xF0                          ; clear lower 4 bit
                     SBR        R16,(1<<CmdData)                  ; set data
                     OUT        LCDPort,R16
                     RCALL      LCD_Enable
                     ANDI       R17,0x0F                          ; clear upper 4 bit
                     SWAP       R17                               ; swap nibble
                     SBR        R17,(1<<CmdData)                  ; set data
                     OUT        LCDPort,R17
                     RCALL      LCD_Enable
                     RCALL      Delay_5ms                         ; wait
                     POP        R17
                     POP        R16
                     RET

; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
; +                  Delay_5ms                                                                                    +
; +                  based on 20Mhz CPU clock                                                                     +
; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
Delay_5ms:           PUSH       R16                               ; save registers
                     PUSH       R17
                     LDI        R16,0x05                          ; R16:= 5 milli seconds
                     MOV        R17,R16                           ; R17:= 5 milli seconds
Delay_5ms_Loop:      LDI        R16,0xFF
                     RCALL      Delay_1us                         ; -->> delay 255 micro seconds
                     RCALL      Delay_1us                         ; -->> delay 255 micro seconds
                     RCALL      Delay_1us                         ; -->> delay 255 micro seconds
                     LDI        R16,0xEB
                     RCALL      Delay_1us                         ; -->> delay 235 micro seconds  (SUM 1000)
                     DEC        R17                               ; R17:= R17 - 1
                     BRNE       Delay_5ms_Loop                    ; -->> R17 == 0x00 ?, No -->> delay loop
                     POP        R17
                     POP        R16
                     RET                                          ; -->> return to caller

Delay_1us:           PUSH       R16                               ; save registers
Delay_1us_Loop:      NOP                                          ;                                                 17  cycl
                     NOP
                     NOP
                     NOP
                     NOP
                     NOP
                     NOP
                     NOP
                     NOP
                     NOP
                     NOP
                     NOP
                     NOP
                     NOP
                     NOP
                     NOP
                     NOP
                     DEC        R16                               ; R16:= R16 - 1                                   1   cycl
                     BRNE       Delay_1us_Loop                    ; -->> R16 == 0x00 ?, No -->> delay loop          1/2 cycl
                     POP        R16
                     RET                                          ; -->> return to caller

                    .END

; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
; + AUTHOR      : Guido Trensch, 2012
; + FILE        : funcgen.asm
; + DESCRIPTION : Function Generator driver assmbler routines
; +
; +               Implements following functions:
; +
; +               ASM_InitAnalogFGen
; +               ASM_SetAnalogMaxF
; +               ASM_SetOutputLevel
; +               ASM_SetWaveSource
; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
                    #include   "equates1284P.inc"

; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
; +                  P O R T   A S S I G N M E N T S                                                              +
; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +

                                                                  ;                                Reed Relais switch table corresponding to
                                                                  ;                                circuit diagram
                                                                  ;                                ---------------------------------------------------
                                                                  ;                                Switch | Sine    Square    Triangle    Puls    Saw
                                                                  ;                                -------+-------------------------------------------
                    .EQU        SW_100Hz,          PA0            ; Cap 1000nF                 RL1 (S1)   | 1of4    ...       ...         ...     ...
                    .EQU        SW_1000Hz,         PA1            ; Cap 100nF                  RL2 (S1)   | 1of4    ...       ...         ...     ...
                    .EQU        SW_10000Hz,        PA2            ; Cap 10nF                   RL3 (S1)   | 1of4    ...       ...         ...     ...
                    .EQU        SW_100000Hz,       PA3            ; Cap 1nF                    RL4 (S1)   | 1of4    ...       ...         ...     ...
                                                                  ;                                       |
                    .EQU        SW_2xFrequency,    PA4            ; double frequency           RL5 (S2)   | -       -         -           -       -
                                                                  ;                                       |
                    .EQU        TSW_UAdjust,       PA5            ; adjustment                 RL6 (S3a)  | 0       -         1           -       1
                    .EQU        SW_SawTooth,       PA6            ; saw tooth                  RL7 (S4)   | 0       0         0           1       1
                    .EQU        SW_Sine,           PA7            ; sine                       RL8 (S3b)  | 1       -         0           -       0
                                                                  ;                                       |
                    .EQU        TSW_SourceSquare,  PD5            ; source is square or ..     RL12(S5)   | 0       1         0           1       0
                    .EQU        TSW_SourceAnalog,  PD6            ; .. analog or DDS           RL13(S7)   | 1       -         1           -       1
                                                                  ;
                                                                  ;                                       | OFF    10%    50%    100%
                                                                  ;                                       |-------------------------------------------
                    .EQU        SW_LVLRL9,         PD2            ; amplification level        RL9 (S6)   |  X      X      -      -
                    .EQU        SW_LVLRL10,        PD3            ; amplification level        RL10(S6)   |  -      X      X      X
                    .EQU        SW_LVLRL11,        PD4            ; amplification level        RL11(S6)   |  -      -      -      X

; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
; +                  E X T E R N A L   F U N C T I O N S                                                          +
; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
; +                  ASM_InitAnalogFGen                                                                           +
; +                                                                                                               +
; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
                    .GLOBAL     ASM_InitAnalogFGen
                    .FUNC       ASM_InitAnalogFGen
ASM_InitAnalogFGen:  PUSH       R16

                     IN         R16,SREG                          ; keep interrupt flag ..
                     PUSH       R16                               ; .. on stack
                     CLI                                          ; disable interrupts

;                    + + + PORT A ALL PINS OUT + + +

                     SER        R16                               ; R16:= x'FF'
                     OUT        DDRA,R16                          ; configure PORTA for output
                     CLR        R16                               ; R16:= x'00'
                     OUT        PORTA,R16                         ; all lines 0

;                    + + + PORT D MIXED OUT + + +

                     IN         R16,DDRD                          ; read current configuration
                     ORI        R16,(1 << SW_LVLRL9) | (1 << SW_LVLRL10) | (1 << SW_LVLRL11) | (1 << TSW_SourceSquare) | (1 << TSW_SourceAnalog)
                     OUT        DDRD,R16                          ; configure lines for output
                     CLR        R16                               ; R16:= x'00'
                     OUT        PORTD,R16                         ; all lines 0

                     POP        R16                               ; restore interrupt flag from stack
                     OUT        SREG,R16

                     POP        R16
                     RET                                          ; -->> return to caller
                    .ENDFUNC

; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
; +                  ASM_SetAnalogMaxF                                                                            +
; +                                                                                                               +
; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
                    .EXTERN     glob_fc_frequency                 ; global variable to hand over a function code
                    .EXTERN     glob_max_frequency                ; global variable to hand over max parameter value
                    .GLOBAL     ASM_SetAnalogMaxF
                    .FUNC       ASM_SetAnalogMaxF
ASM_SetAnalogMaxF:   PUSH       R16
                     PUSH       R17
                     PUSH       R30
                     PUSH       R31

                     IN         R16,SREG                          ; keep interrupt flag ..
                     PUSH       R16                               ; .. on stack
                     CLI                                          ; disable interrupts

                     CBI        PORTA,SW_100Hz                    ; reset all switches
                     CBI        PORTA,SW_1000Hz
                     CBI        PORTA,SW_10000Hz
                     CBI        PORTA,SW_100000Hz 
                     CBI        PORTA,SW_2xFrequency

                     LDS        R16,glob_fc_frequency
                     DEC        R16                               ; take MIN_F into account
                     LDS        R17,glob_max_frequency            ; check max value
                     CP         R16,R17
                     BRSH       SetAnalogMaxF_1000

                     LDI        R30,lo8(SetAnalogMaxF_BTAB)
                     LDI        R31,hi8(SetAnalogMaxF_BTAB)
                     LSR        R31                               ; address R31:R30 is divided by two     0 -> R31 -> Carry
                     ROR        R30                               ;                                       Carry -> R30 ->
                     ADD        R30,R16                           ; 16 bit address offset addition, low byte
                     CLR        R16                               ; clear register
                     ADC        R31,R16                           ; add high byte which is just the carry flag
                     IJMP                                         ; indirect jumpo pointed to by R31:R30 (Z)

                                                                  ; branch table                function_code
SetAnalogMaxF_BTAB:  RJMP       SetAnalogMaxF_100                  ;                 F100Hz    =   0
                     RJMP       SetAnalogMaxF_200                  ;                 F200Hz    =   1
                     RJMP       SetAnalogMaxF_1000                 ;                 F1000Hz   =   2
                     RJMP       SetAnalogMaxF_2000                 ;                 F2000Hz   =   3
                     RJMP       SetAnalogMaxF_10000                ;                 F10000Hz  =   4
                     RJMP       SetAnalogMaxF_20000                ;                 F20000Hz  =   5
                     RJMP       SetAnalogMaxF_100000               ;                 F100000Hz =   6
                     RJMP       SetAnalogMaxF_200000               ;                 F200000Hz =   7

SetAnalogMaxF_100:   SBI        PORTA,SW_100Hz
                     RJMP       SetAnalogMaxF_Exit

SetAnalogMaxF_200:   SBI        PORTA,SW_100Hz
                     SBI        PORTA,SW_2xFrequency
                     RJMP       SetAnalogMaxF_Exit

SetAnalogMaxF_1000:  SBI        PORTA,SW_1000Hz
                     RJMP       SetAnalogMaxF_Exit

SetAnalogMaxF_2000:  SBI        PORTA,SW_1000Hz
                     SBI        PORTA,SW_2xFrequency
                     RJMP       SetAnalogMaxF_Exit

SetAnalogMaxF_10000: SBI        PORTA,SW_10000Hz
                     RJMP       SetAnalogMaxF_Exit

SetAnalogMaxF_20000: SBI        PORTA,SW_10000Hz
                     SBI        PORTA,SW_2xFrequency
                     RJMP       SetAnalogMaxF_Exit

SetAnalogMaxF_100000:SBI        PORTA,SW_100000Hz
                     RJMP       SetAnalogMaxF_Exit

SetAnalogMaxF_200000:SBI        PORTA,SW_100000Hz
                     SBI        PORTA,SW_2xFrequency
;                    RJMP       SetAnalogMaxF_Exit

SetAnalogMaxF_Exit:  POP        R16                               ; restore interrupt flag from stack
                     OUT        SREG,R16

                     POP        R31
                     POP        R30
                     POP        R17
                     POP        R16
                     RET                                          ; -->> return to caller
                    .ENDFUNC

; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
; +                  ASM_SetOutputLevel                                                                           +
; +                                                                                                               +
; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
                    .EXTERN     glob_fc_level                     ; global variable to hand over a function code
                    .EXTERN     glob_max_level                    ; global variable to hand over max parameter value
                    .GLOBAL     ASM_SetOutputLevel
                    .FUNC       ASM_SetOutputLevel
ASM_SetOutputLevel:  PUSH       R16
                     PUSH       R17
                     PUSH       R30
                     PUSH       R31

                     IN         R16,SREG                          ; keep interrupt flag ..
                     PUSH       R16                               ; .. on stack
                     CLI                                          ; disable interrupts

                     CBI        PORTD,SW_LVLRL10
                     CBI        PORTD,SW_LVLRL11
                     SBI        PORTD,SW_LVLRL9                   ; this combination turns output off

                     LDS        R16,glob_fc_level
                     DEC        R16                               ; take MIN_LVL into account
                     LDS        R17,glob_max_level                ; check max value
                     CP         R16,R17
                     BRSH       SetOutputLevel100

                     LDI        R30,lo8(SetOutputLevel_BTAB)
                     LDI        R31,hi8(SetOutputLevel_BTAB)
                     LSR        R31                               ; address R31:R30 is divided by two     0 -> R31 -> Carry
                     ROR        R30                               ;                                       Carry -> R30 ->
                     ADD        R30,R16                           ; 16 bit address offset addition, low byte
                     CLR        R16                               ; clear register
                     ADC        R31,R16                           ; add high byte which is just the carry flag
                     IJMP                                         ; indirect jumpo pointed to by R31:R30 (Z)

                                                                  ; branch table             function_code
SetOutputLevel_BTAB: RJMP       SetOutputLevelOFF                 ;                 LVLOFF = 0
                     RJMP       SetOutputLevel10                  ;                 LVL10  = 1
                     RJMP       SetOutputLevel50                  ;                 LVL50  = 2
                     RJMP       SetOutputLevel100                 ;                 LVL100 = 2

SetOutputLevelOFF:   RJMP       SetOutputLevel_Exit

SetOutputLevel10:    SBI        PORTD,SW_LVLRL10
                     RJMP       SetOutputLevel_Exit

SetOutputLevel50:    SBI        PORTD,SW_LVLRL10
                     CBI        PORTD,SW_LVLRL9
                     RJMP       SetOutputLevel_Exit

SetOutputLevel100:   SBI        PORTD,SW_LVLRL10
                     CBI        PORTD,SW_LVLRL9
                     SBI        PORTD,SW_LVLRL11
;                    RJMP       SetOutputLevel_Exit

SetOutputLevel_Exit: POP        R16                               ; restore interrupt flag from stack
                     OUT        SREG,R16

                     POP        R31
                     POP        R30
                     POP        R17
                     POP        R16
                     RET                                          ; -->> return to caller
                    .ENDFUNC

; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
; +                  ASM_SetWaveSource                                                                            +
; +                                                                                                               +
; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
                    .EXTERN     glob_wave_form                    ; global variable to hand over a function code
                    .EXTERN     glob_num_wave_form                ; global variable to hand over max parameter value
                    .GLOBAL     ASM_SetWaveSource
                    .FUNC       ASM_SetWaveSource
ASM_SetWaveSource:   PUSH       R16
                     PUSH       R17
                     PUSH       R30
                     PUSH       R31

                     IN         R16,SREG                          ; keep interrupt flag ..
                     PUSH       R16                               ; .. on stack
                     CLI                                          ; disable interrupts

                     CBI        PORTA,TSW_UAdjust
                     CBI        PORTA,SW_SawTooth
                     CBI        PORTA,SW_Sine
                     CBI        PORTD,TSW_SourceSquare
                     CBI        PORTD,TSW_SourceAnalog

                     LDS        R16,glob_wave_form
                     DEC        R16                               ; take MIN_WAVE_FORM into account
                     LDS        R17,glob_num_wave_form            ; check max value
                     CP         R16,R17
                     BRSH       SetWaveSource_Sine

                     LDI        R30,lo8(SetWaveSource_BTAB)
                     LDI        R31,hi8(SetWaveSource_BTAB)
                     LSR        R31                               ; address in R31:R30 is divided by two   0 -> R31 -> Carry
                     ROR        R30                               ;                                        Carry -> R30 ->
                     ADD        R30,R16                           ; 16 bit address offset addition, low byte
                     CLR        R16                               ; clear register
                     ADC        R31,R16                           ; add high byte which is just the carry flag
                     IJMP                                         ; -->> indirect jump pointed to by R31:R30(Z)into BTAB

                                                                  ; branch table                function_code
SetWaveSource_BTAB:  RJMP       SetWaveSource_Sine                ;                 Sine     =   0
                     RJMP       SetWaveSource_Sqr                 ;                 Square   =   1
                     RJMP       SetWaveSource_Tri                 ;                 Triangle =   2
                     RJMP       SetWaveSource_Puls                ;                 Puls     =   3
                     RJMP       SetWaveSource_Saw                 ;                 SawTooth =   4
                     RJMP       SetWaveSource_DDS                 ;                 DDS      =   5

SetWaveSource_Sine:  SBI        PORTA,SW_Sine
                     SBI        PORTD,TSW_SourceAnalog
                     RJMP       SetWaveSource_Exit

SetWaveSource_Sqr:   SBI        PORTD,TSW_SourceSquare
                     RJMP       SetWaveSource_Exit

SetWaveSource_Tri:   SBI        PORTA,TSW_UAdjust
                     SBI        PORTD,TSW_SourceAnalog
                     RJMP       SetWaveSource_Exit

SetWaveSource_Puls:  SBI        PORTA,SW_SawTooth
                     SBI        PORTD,TSW_SourceSquare
                     RJMP       SetWaveSource_Exit

SetWaveSource_Saw:   SBI        PORTA,TSW_UAdjust
                     SBI        PORTA,SW_SawTooth
                     SBI        PORTD,TSW_SourceAnalog

SetWaveSource_DDS:   SBI        PORTA,SW_Sine                    ; just to have a defined signal and no free running oscillator
                     RJMP       SetWaveSource_Exit
;                    RJMP       SetWaveSource_Exit

SetWaveSource_Exit:  POP        R16                              ; restore interrupt flag from stack
                     OUT        SREG,R16

                     POP        R31
                     POP        R30
                     POP        R17
                     POP        R16
                     RET                                         ; -->> return to caller
                    .ENDFUNC

                    .END

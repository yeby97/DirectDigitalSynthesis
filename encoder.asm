; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
; + AUTHOR      : Guido Trensch, 2012
; + FILE        : encoder.asm
; + DESCRIPTION : Incemental encoder driver
; +
; +               Implements following functions:
; +
; +               ASM_InitEncoder
; +               ASM_InterruptEncSW
; +               ASM_InterruptEncAB
; +               ASM_LockEncAB
; +               ASM_ReleaseEncAB
; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
                     #include   "equates1284P.inc"

; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
; +                  L C D   D I S P L A Y   P O R T   S E T T I N G                                              +
; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
                     .EQU       EncABPin,     PINB
                     .EQU       EncABPort,    PORTB
                     .EQU       EncABDDReg,   DDRB
                     .EQU       EncA,         PB0
                     .EQU       EncB,         PB1
                     .EQU       EncSWPort,    PORTD
                     .EQU       EncSWDDReg,   DDRD
                     .EQU       EncSW,        PD7                 ; if changed, take care of corresponding interrupt

; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
; +                  E X T E R N A L   F U N C T I O N S                                                          +
; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
; +                  ASM_InitEncoder                                                                              +
; +                                                                                                               +
; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
                    .GLOBAL     ASM_InitEncoder
                    .FUNC       ASM_InitEncoder
ASM_InitEncoder:     PUSH       R16                               ; save register
                     PUSH       R30
                     PUSH       R31

                     IN         R16,SREG                          ; keep interrupt flag ..
                     PUSH       R16                               ; .. on stack
                     CLI                                          ; disable interrupts

;                    + + + INITIALIZE ENCODER PORTS + + +

                     IN         R16,EncABDDReg                    ; read current configuration
                     CBR        R16,EncA                          ; clear corresponding bit
                     CBR        R16,EncB                          ; clear corresponding bit
                     OUT        EncABDDReg,R16                    ; configure lines for input 

;                    IN         R16,EncSWDDReg                    ; read current configuration
;                    CBR        R16,EncSW                         ; clear corresponding bit
;                    OUT        EncSWDDReg,R16                    ; configure line for input 

                     LDI        R16,(1<<PCIE3) | (1<<PCIE1)       ; enable pin change interrupt 3 and 1 ..
                     STS        PCICR,R16                         ; PCINT31 -> PCIE3, PCINT8 -> PCIE1
                     LDI        R16,(1<<PCINT31)                  ; mask/enable pin PCINT31 (EncSW PD7)
                     STS        PCMSK3,R16
                     LDI        R16,(1<<PCINT8)                   ; mask/enable pin PCINT8 (EncA PB0)
                     STS        PCMSK1,R16

                     POP        R16                               ; restore interrupt flag from stack
                     OUT        SREG,R16

                     POP        R31
                     POP        R30
                     POP        R16                               ; restore register
                     RET
                    .ENDFUNC

; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
; +                  ASM_InterruptEncSW                                                                           +
; +                  In case of an interrupt call the registered callback function addressed by                   +
; +                  glob_pCallBackFunc_Switch                                                                    +
; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
                    .EXTERN     glob_pCallBackFunc_Switch        ; global variable, callback function pointer
                    .GLOBAL     ASM_InterruptEncSW
                    .FUNC       ASM_InterruptEncSW
ASM_InterruptEncSW:  PUSH       R16
                     PUSH       R30
                     PUSH       R31

                     RCALL      Delay_100ms                      ; de-bounce switch
                     RCALL      Delay_100ms                      ; de-bounce switch
                     RCALL      Delay_100ms                      ; de-bounce switch

                     LDS        R30,glob_pCallBackFunc_Switch
                     LDS        R31,glob_pCallBackFunc_Switch+1  ; load Z to prepare ICALL
                     ICALL                                       ; call the callback function

                     SBI        PCIFR,PCIF3                      ; clear bit (Bit is set!) signaling the interrupt ..
                                                                 ; .. otherwise pin logic level changes within this routine ..
                                                                 ; .. causing subsequent interrupts
                     POP        R31
                     POP        R30
                     POP        R16
                     RET
                    .ENDFUNC

; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
; +                  ASM_InterruptEncAB                                                                           +
; +                  In case of an interrupt call one of the registered callback functions addressed by           +
; +                  glob_pCallBackFunc_Left                                                                      +
; +                  glob_pCallBackFunc_Right                                                                     +
; +                  The function chosen depends on the rotation direction of the encoder.                        +
; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
                    .EXTERN     glob_pCallBackFunc_Left          ; global variable, callback function pointer
                    .EXTERN     glob_pCallBackFunc_Right         ; global variable, callback function pointer
                    .GLOBAL     ASM_InterruptEncAB
                    .FUNC       ASM_InterruptEncAB
ASM_InterruptEncAB:  PUSH       R16
                     PUSH       R17
                     PUSH       R30
                     PUSH       R31

                     RCALL      Delay_6ms                       ; de-bounce switch
; +---------------------------------------------------------------------------------------------------------------+
; |                                                                                                               |
; |     ENCODER ROTATION DIRECTION DETECTION ALGORITHM                                                            |
; |                                                                                                               |
; |       LEFT Rotate                 |        RIGHT Rotate                                                       |
; |                                   |                                                                           |
; |  A   1 1 0 0 1 1 0 0 1 1          |     A   1 0 0 1 1 0 0 1 1                                                 |
; |      ---+   +---+   +---          |         -+   +---+   +---                                                 |
; |         |   |   |   |             |          |   |   |   |                                                    |
; |         +---+   +---+             |          +---+   +---+                                                    |
; |  B   0 1 1 0 0 1 1 0 0            |     B   0 0 1 1 0 0 1 1 0                                                 |
; |       +---+   +---+   +-          |            +---+   +---+                                                  |
; |       |   |   |   |   |           |            |   |   |   |                                                  |
; |      -+   +---+   +---+           |         ---+   +---+   +-                                                 |
; |      | | | | | | | | | |          |         | | | | | | | | |                                                 |
; |  C = 1 3 2 0 1 3 2 0 1 3          |     C = 1 0 2 3 1 0 2 3 1                                                 |
; |                                   |                                                                           |
; |  C = B*2^1 + A*2^0                |                                                                           |
; |                                   |                                                                           |
; |    Possible transitions of C      |                                                                           |
; |      1->3->2->0->1 ...            |         1->0->2->3->1 ...                                                 |
; |                                                                                                               |
; |    switch ( C ) {                                                                                             |
; |     0: WAIT_FOR ( C != 0 )                in other words WAIT_FOR ( A or B ), a change of the current state   |
; |        if( C == 1 ) status = LEFT                                                                             |
; |        else         status = RIGHT                                                                            |
; |        break                                                                                                  |
; |     1: WAIT_FOR ( C != 1 )                in other words WAIT_FOR ( !A or B ), a change of the current state  |
; |        if( C == 3 ) status = LEFT                                                                             |
; |        else         status = RIGHT                                                                            |
; |        break                                                                                                  |
; |     2: WAIT_FOR ( C != 2 )                in other words WAIT_FOR ( A or !B ), a change of the current state  |
; |        if( C == 0 ) status = LEFT                                                                             |
; |        else         status = RIGHT                                                                            |
; |        break                                                                                                  |
; |     3: WAIT_FOR ( C != 3 )                in other words WAIT_FOR ( !A or !B ), a change of the current state |
; |        if( C == 2 ) status = LEFT                                                                             |
; |        else         status = RIGHT                                                                            |
; |        break                                                                                                  |
; |    }                                                                                                          |
; +---------------------------------------------------------------------------------------------------------------+

;                    + + + READ ENCODER PORT AND ANALYZE A B SWITCH SETTING + + +

                     IN         R16,EncABPin                     ; read encoder port
                     ANDI       R16,(1<<EncB) | (1<<EncA)        ; clear R16 except A and B

                     TST        R16                              ; !B and !A ?
                     BREQ       EncAB_CASE0                      ; ->> case 0

                     MOV        R17,R16                          ; make a copy of the encoder switch settings
                     SUBI       R17,(0<<EncB) | (1<<EncA)        ; !B and A ?
                     BREQ       EncAB_CASE1                      ; ->> case 1

                     MOV        R17,R16                          ; make a copy of the encoder switch settings
                     SUBI       R17,(1<<EncB) | (0<<EncA)        ; B and !A ?
                     BREQ       EncAB_CASE2                      ; ->> case 2

                     MOV        R17,R16                          ; make a copy of the encoder switch settings
                     SUBI       R17,(1<<EncB) | (1<<EncA)        ; B and A ?
                     BREQ       EncAB_CASE3                      ; ->> case 3

                     RJMP       EncAB_Return                     ; just for robustness

;                    + + + READ IN A SUBSEQUENT VALUE AND ANALYZE THE TRANSITION + + +

EncAB_CASE0:         IN         R16,EncABPin                     ; read encoder port <---------------------------+
                     ANDI       R16,(1<<EncB) | (1<<EncA)        ; clear R16 except A and B                      |
                     TST        R16                              ; !B and !A ?                                   |
                     BREQ       EncAB_CASE0                      ; ->> wait / poll ------------------------------+
                     SUBI       R16,(0<<EncB) | (1<<EncA)        ; !B and A ( transition 0->2 ? )
                     BREQ       EncAB_LeftRotate                 ; ->> left
                     RJMP       EncAB_RightRotate                ; ->> right

EncAB_CASE1:         IN         R16,EncABPin                     ; read encoder port <---------------------------+
                     ANDI       R16,(1<<EncB) | (1<<EncA)        ; clear R16 except A and B                      |
                     MOV        R17,R16                          ; make a copy of the encoder switch settings    |
                     SUBI       R17,(0<<EncB) | (1<<EncA)        ; !B and A ?                                    |
                     BREQ       EncAB_CASE1                      ; ->> wait / poll ------------------------------+
                     SUBI       R16,(1<<EncB) | (1<<EncA)        ; B and A ( transition 1->3 ? )
                     BREQ       EncAB_LeftRotate                 ; ->> left
                     RJMP       EncAB_RightRotate                ; ->> right

EncAB_CASE2:         IN         R16,EncABPin                     ; read encoder port <---------------------------+
                     ANDI       R16,(1<<EncB) | (1<<EncA)        ; clear R16 except A and B                      |
                     MOV        R17,R16                          ; make a copy of the encoder switch settings    |
                     SUBI       R17,(1<<EncB) | (0<<EncA)        ; B and !A ?                                    |
                     BREQ       EncAB_CASE2                      ; ->> wait / poll ------------------------------+
                     TST        R16                              ; !B and !A ( transition 2->0 ? )
                     BREQ       EncAB_LeftRotate                 ; ->> left
                     RJMP       EncAB_RightRotate                ; ->> right

EncAB_CASE3:         IN         R16,EncABPin                     ; read encoder port <---------------------------+
                     ANDI       R16,(1<<EncB) | (1<<EncA)        ; clear R16 except A and B                      |
                     MOV        R17,R16                          ; make a copy of the encoder switch settings    |
                     SUBI       R17,(1<<EncB) | (1<<EncA)        ; B and A ?                                     |
                     BREQ       EncAB_CASE3                      ; ->> wait / poll ------------------------------+
                     SUBI       R16,(1<<EncB) | (0<<EncA)        ; B and !A ( transition 3->2 ? )
                     BREQ       EncAB_LeftRotate                 ; ->> left
                     RJMP       EncAB_RightRotate                ; ->> right

;                    + + + CALL THE LEFT- OR RIGHT-Rotate CALLBACK FUNCTIONS + + +

EncAB_LeftRotate:    LDS        R30,glob_pCallBackFunc_Left
                     LDS        R31,glob_pCallBackFunc_Left+1    ; load Z to prepare ICALL
                     ICALL                                       ; call the callback function
                     RJMP       EncAB_Return

EncAB_RightRotate:   LDS        R30,glob_pCallBackFunc_Right
                     LDS        R31,glob_pCallBackFunc_Right+1   ; load Z to prepare ICALL
                     ICALL                                       ; call the callback function
                     RJMP       EncAB_Return

;                    + + + RETURN TO THE SIGNAL HANDLER WRAPPER FUNCTION + + +

EncAB_Return:        SBI        PCIFR,PCIF1                      ; clear bit (Bit is set!) signaling the interrupt ..
                                                                 ; .. otherwise pin logic level changes within this routine ..
                                                                 ; .. causing subsequent interrupts
                     POP        R31
                     POP        R30
                     POP        R17
                     POP        R16
                     RET
                    .ENDFUNC

; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
; +                  ASM_LockEncAB                                                                                +
; +                                                                                                               +
; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
                    .GLOBAL     ASM_LockEncAB
                    .FUNC       ASM_LockEncAB
ASM_LockEncAB:       PUSH       R16                               ; save register
;                    LDI        R16,(0<<PCINT8)                   ; demask/disable pin PCINT8 (EncA PB0)
                     CLR        R16                               ; demask/disable pin PCINT8 (EncA PB0)
                     STS        PCMSK1,R16
                     POP        R16                               ; restore register
                     RET
                    .ENDFUNC

; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
; +                  ASM_ReleaseEncAB                                                                             +
; +                                                                                                               +
; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
                    .GLOBAL     ASM_ReleaseEncAB
                    .FUNC       ASM_ReleaseEncAB
ASM_ReleaseEncAB:    PUSH       R16                               ; save register
                     LDI        R16,(1<<PCINT8)                   ; mask/enable pin PCINT8 (EncA PB0)
                     STS        PCMSK1,R16
                     SBI        PCIFR,PCIF1                       ; clear bit (Bit is set!) signaling the interrupt ..
                                                                  ; .. otherwise pin logic level changes within this routine ..
                                                                  ; .. causing subsequent interrupts
                     POP        R16                               ; restore register
                     RET
                    .ENDFUNC

; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
; +                  I N T E R N A L   S U B R O U T I N E S                                                      +
; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
; +                  Delay_100ms                                                                                  +
; +                  Loop for app. 100 milli seconds                                                              +
; +                  based on 20Mhz CPU clock                                                                     +
; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
Delay_100ms:         PUSH       R16                               ; save registers
                     PUSH       R17
                     LDI        R16,0x64                          ; R16:= 100 milli seconds
                     MOV        R17,R16                           ; R17:= 100 milli seconds
Delay_100ms_Loop:    LDI        R16,0xFF
                     RCALL      Delay_us                          ; -->> delay 255 micro seconds
                     RCALL      Delay_us                          ; -->> delay 255 micro seconds
                     RCALL      Delay_us                          ; -->> delay 255 micro seconds
                     LDI        R16,0xEB
                     RCALL      Delay_us                          ; -->> delay 235 micro seconds  (SUM 1000)
                     DEC        R17                               ; R17:= R17 - 1
                     BRNE       Delay_100ms_Loop                  ; -->> R17 == 0x00 ?, No -->> delay loop
                     POP        R17                               ; restore registers
                     POP        R16
                     RET                                          ; -->> return to caller

; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
; +                  Delay_10ms                                                                                  +
; +                  Loop for app. 6 milli seconds                                                              +
; +                  based on 20Mhz CPU clock                                                                     +
; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
Delay_6ms:           PUSH       R16                               ; save registers
                     PUSH       R17
                     LDI        R16,0x06                          ; R16:= 10 milli seconds
                     MOV        R17,R16                           ; R17:= 10 milli seconds
Delay_6ms_Loop:      LDI        R16,0xFF
                     RCALL      Delay_us                          ; -->> delay 255 micro seconds
                     RCALL      Delay_us                          ; -->> delay 255 micro seconds
                     RCALL      Delay_us                          ; -->> delay 255 micro seconds
                     LDI        R16,0xEB
                     RCALL      Delay_us                          ; -->> delay 235 micro seconds  (SUM 1000)
                     DEC        R17                               ; R17:= R17 - 1
                     BRNE       Delay_6ms_Loop                    ; -->> R17 == 0x00 ?, No -->> delay loop
                     POP        R17                               ; restore registers
                     POP        R16
                     RET                                          ; -->> return to caller

; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
; +                  Delay_us                                                                                     +
; +                  Loop for app. <R16> micro seconds                                                            +
; +                  based on 20Mhz CPU clock                                                                     +
; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
Delay_us:            PUSH       R16                               ; save registers
Delay_1us_Loop:      NOP                                          ;                                                  1  cycl
                     NOP                                          ;                                                  1  cycl
                     NOP                                          ;                                                  1  cycl
                     NOP                                          ;                                                  1  cycl
                     NOP                                          ;                                                  1  cycl
                     NOP                                          ;                                                  1  cycl
                     NOP                                          ;                                                  1  cycl
                     NOP                                          ;                                                  1  cycl
                     NOP                                          ;                                                  1  cycl
                     NOP                                          ;                                                  1  cycl
                     NOP                                          ;                                                  1  cycl
                     NOP                                          ;                                                  1  cycl
                     NOP                                          ;                                                  1  cycl
                     NOP                                          ;                                                  1  cycl
                     NOP                                          ;                                                  1  cycl
                     NOP                                          ;                                                  1  cycl
                     NOP                                          ;                                                  1  cycl
                     DEC        R16                               ; R16:= R16 - 1                                    1  cycl
                     BRNE       Delay_1us_Loop                    ; -->> R16 == 0x00 ?, No -->> delay loop          1/2 cycl
                     POP        R16                               ; restore registers
                     RET                                          ; -->> return to caller

                    .END

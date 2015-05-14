; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
; + AUTHOR      : Guido Trensch, 2012
; + FILE        : serial128.asm
; + DESCRIPTION : USART driver assmbler routines for the RS232 serial debug interface
; +
; +               Implements following functions:
; +
; +               ASM_InitUSART0
; +               ASM_SendString
; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
                    #include   "equates1284P.inc"

; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
; +                  U S A R T   S E T T I N G S                                                                  +
; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
                    .EQU        CLK,      20000000                       ; note: deactivate fuse CKDIV8 (ATMega1284P)
                    .EQU        BAUDRATE, 19200

; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
; +                  E X T E R N A L   F U N C T I O N S                                                          +
; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
; +                  ASM_InitUSART0                                                                               +
; +                                                                                                               +
; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
                    .GLOBAL     ASM_InitUSART0
                    .FUNC       ASM_InitUSART0
ASM_InitUSART0:      PUSH       R16                                       ; save register
                     PUSH       R17

                     IN         R16,SREG                                  ; keep interrupt flag ..
                     PUSH       R16                                       ; .. on stack
                     CLI                                                  ; disable interrupts

                     LDI        R16,CLK/(16*BAUDRATE) - 1                 ; calculate baudrate
                     STS        UBRR0L,R16                                ; UBRRnL:= baudrate  (UBRRnH:= 0 after reset)
                                                                          ; does not work with OUT, UBRR0L > 64 (memory mapped)
                     STS        UCSR0A,U2X0                               ; 2 * baudrate
                     LDI        R16,(1<<USBS0)|(3<<UCSZ00)                ; prepare control reg UCSRC
                     STS        UCSR0C,R16                                ; UCSRC:= async (default), 2 stop, 8 data
                                                                          ; does not work with OUT, UCSR0C > 64 (memory mapped)
                     LDI        R16,(1<<RXCIE0)|(1<<RXEN0)|(1<<TXEN0)     ; prepare control reg UCSRB
                     STS        UCSR0B,R16                                ; UCSRB:= receiver on, sender on
                     LDS        R16,UDR0                                  ; empty data register (Usart Data Register)

                     POP        R16                                       ; restore interrupt flag from stack
                     OUT        SREG,R16

                     POP        R17
                     POP        R16                                       ; restore register
                     RET
                    .ENDFUNC

; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
; +                  ASM_SendString                                                                               +
; +                                                                                                               +
; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
                    .EXTERN     glob_pData                                ; global variable, pointer to string to be sent
                    .EXTERN     glob_lenData                              ; global variable, length send data (255 max)
                    .GLOBAL     ASM_SendString
                    .FUNC       ASM_SendString
ASM_SendString:      PUSH       R16                                       ; save registers
                     PUSH       R17
                     PUSH       R18
                     PUSH       R26
                     PUSH       R27

                     IN         R16,SREG                                  ; keep interrupt flag ..
                     PUSH       R16                                       ; .. on stack
                     CLI                                                  ; disable interrupts

                     LDS        R16,glob_lenData                          ; R16:= (# bytes in send buffer)
                     TST        R16                                       ; null ?
                     BREQ       SendString_Return                         ; ->> Y, return to caller
                     LDS        R26,glob_pData                            ; X low_address(send buffer)
                     LDS        R27,glob_pData + 1                        ; X high_address(send buffer)

SendString_Loop:     LD         R17,X+                                    ; R17:= (send buffer)[0] and X:= X + 1 (post increment) 
SendString_Free:     LDS        R18,UCSR0A                                ; load UCSR0A into R18
                     SBRS       R18,UDRE0                                 ; ->> data register empty flag set ? Y, skip 
                     RJMP       SendString_Free                           ; ->> N, spin loop
                     STS        UDR0,R17                                  ; send byte
                     DEC        R16                                       ; R16:= R16 - 1, (# bytes in send buffer) - 1
                     BRNE       SendString_Loop                           ; ->> null ?, N, next byte

SendString_Return:   POP        R16                                       ; restore interrupt flag from stack
                     OUT        SREG,R16

                     POP        R27                                       ; restore registers
                     POP        R26
                     POP        R17
                     POP        R18
                     POP        R16
                     RET                                                  ; -->> return to caller
                    .ENDFUNC

                    .END

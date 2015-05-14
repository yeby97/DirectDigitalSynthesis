; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
; + AUTHOR      : Guido Trensch, 2012
; + FILE        : delay.asm
; + DESCRIPTION : Time delay routines
; +
; +               Implements following functions:
; +
; +               ASM_Delay_Seconds
; +               ASM_Delay_Millis
; +               ASM_Delay_Micros
; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
                     #include   "equates1284P.inc"

; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
; +                  ASM_Delay_Seconds                                                                            +
; +                  INPUT: R16 .. delay time in seconds                                                          +
; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
                    .EXTERN     glob_seconds                      ; global variable
                    .GLOBAL     ASM_Delay_Seconds
                    .FUNC       ASM_Delay_Seconds
ASM_Delay_Seconds:   PUSH       R16                               ; save registers
                     PUSH       R17
                     LDS        R16,glob_seconds
                     MOV        R17,R16                           ; R17:= milliseconds
DelayLoopSeconds:    LDI        R16,0xFF
                     CALL       ASM_Delay_Millis                  ; -->> delay 255 milli seconds
                     CALL       ASM_Delay_Millis                  ; -->> delay 255 milli seconds
                     CALL       ASM_Delay_Millis                  ; -->> delay 255 milli seconds
                     LDI        R16,0xEB
                     CALL       ASM_Delay_Millis                  ; -->> delay 235 milli seconds  (SUM 1000)
                     DEC        R17                               ; R17:= R17 - 1
                     BRNE       DelayLoopSeconds                  ; -->> R17 == 0x00 ?, No -->> delay loop
                     POP        R17
                     POP        R16
                     RET                                          ; -->> return to caller
                    .ENDFUNC

; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
; +                  ASM_Delay_Millis                                                                             +
; +                  INPUT: R16 .. delay time in milli seconds                                                    +
; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
                    .EXTERN     glob_milliseconds                 ; global variable
                    .GLOBAL     ASM_Delay_Millis
                    .FUNC       ASM_Delay_Millis
ASM_Delay_Millis:    PUSH       R16                               ; save registers
                     PUSH       R17
                     LDS        R16,glob_milliseconds
                     MOV        R17,R16                           ; R17:= milliseconds
DelayLoopMillis:     LDI        R16,0xFF
                     CALL       ASM_Delay_Micros                  ; -->> delay 255 micro seconds
                     CALL       ASM_Delay_Micros                  ; -->> delay 255 micro seconds
                     CALL       ASM_Delay_Micros                  ; -->> delay 255 micro seconds
                     LDI        R16,0xEB
                     CALL       ASM_Delay_Micros                  ; -->> delay 235 micro seconds  (SUM 1000)
                     DEC        R17                               ; R17:= R17 - 1
                     BRNE       DelayLoopMillis                   ; -->> R17 == 0x00 ?, No -->> delay loop
                     POP        R17
                     POP        R16
                     RET                                          ; -->> return to caller
                    .ENDFUNC

; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
; +                  ASM_Delay_Micros                                                                             +
; +                  delay[microsec] = [<R16>*(20 cycl x 1/20 sec)]                                               +
; +                  INPUT: R16 .. delay time in micro seconds                                                    +
; + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
                    .EXTERN     glob_microseconds                 ; global variable
                    .GLOBAL     ASM_Delay_Micros
                    .FUNC       ASM_Delay_Micros
ASM_Delay_Micros:    PUSH       R16                               ; save registers
                     LDS        R16,glob_microseconds                                                               CYCLES
DelayLoopMicros:     NOP                                          ;                                                 17  cycl
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
                     BRNE       DelayLoopMicros                   ; -->> R16 == 0x00 ?, No -->> delay loop          1/2 cycl
                     POP        R16
                     RET                                          ; -->> return to caller
                    .ENDFUNC

                    .END
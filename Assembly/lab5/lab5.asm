;; program to test trap x38
;; this program must be loaded along with trap38.obj and set_trap.obj
 
        .ORIG    x3000
 
LOOP    TRAP     x38           ;read a char and echo a *
                               ;(char comes back in R0)
 
; Check if this is the end-of-line character
        LD       R1, LF         ;ASCII code for Line Feed
        NOT      R1, R1
        ADD      R1, R1, #1    ;negate the LF...
        ADD      R1, R0, R1    ;...to subtract it from the char
        BRZ      DONE          ; It is an LF – we are done
 
        LD       R1, CR         ; Do the same with Carriage Return
        NOT      R1, R1
        ADD      R1, R1, #1
        ADD      R1, R0, R1
        BRZ      DONE           ; It is a CR
 
        BR       LOOP           ;not end of line, so read another
 
DONE     HALT                   ;newline detected, so halt
 
LF       .FILL   x0A            ;ASCII code for Line Feed
CR      .FILL   x0D             ;ASCII code for Carriage Return
         .END
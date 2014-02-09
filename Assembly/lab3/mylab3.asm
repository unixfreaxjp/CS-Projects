;
; Program to multiply two integers. 
; Before execution, an integer must be stored in NUMBER.

; This version is modified from the original to take a number from the memory location MULTADDR, and multiply it with the value NUMBER
; I also assumed that being able to multiple "any number" also meant accounting for correct sign, so this program correctly computes signed multiplication as well
 
        .ORIG   x3050
	LD	R4, N_TWO     ; set R4 to -1. Will be used check if either inputs are negative
			      ; if R4 is zero, the result should be negated
        LDI     R1,MULTADDR   ; load the first number from the location pointed to by MULTADDR
	BRzp    LOAD2         ; branch to LOAD2 if first number is (not) negative
	ADD	R4, R4, #1    ; first number is negative so add one to R4   
	NOT	R1, R1	      ; make the number positive
	ADD 	R1, R1, #1   

LOAD2   LD      R2,NUMBER     ; load the second number
	BRzp    MULT          ; check if second number is (not) negative
        ADD	R4, R4, #1    ; second number is negative so add one to R4
	NOT     R2, R2        ; make the number positive
	ADD     R2, R2, #1

MULT    AND     R3,R3,#0      ; Clear R3. It will
                              ; contain the product.
; The inner loop
AGAIN   ADD     R3,R3,R2
        ADD     R1,R1,#-1     ; R1 keeps track of
        BRp     AGAIN         ; the iterations
;
	ADD R4, R4, #1        ; check if R4 is -1 by adding 1 and checking if the result is zero
	BRnp EXIT             ; if R4 was negative or positive, skip to end (means n1 and n2 had the same sign)
	NOT R3, R3            ; make R3 negative by computing its two's complement
	ADD R3, R3, #1
EXIT    HALT
;
NUMBER  .BLKW   1           
MULTADDR     .FILL   x4000    ; the address of the second number to multiply by
N_TWO   .FILL x-0002          ; constant for -2

        .END
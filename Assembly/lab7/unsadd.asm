; Program to test unsigned addition

	.ORIG x3000

; First exercise Case 1
	LD R0, AH
	LD R1, AL		; 1,000,000 + 1,500,000 = 2,500,000
	ld	r2, BH
	ld	r3, BL
	JSR	ADD32

; First exercise Case 1
	LD R0, AH2		; xffffffff + x1 = x0
	LD R1, AL2
	ld	r2, BH2
	ld	r3, BL2
	JSR	ADD32

; First exercise Case 1
	LD R0, AH3
	LD R1, AL2		; x7fffffff + x1 = x0
	ld	r2, BH2
	ld	r3, BL2
	JSR	ADD32

	HALT

; Some sample data
AH	.fill	b0000000000001111 	; dec 1,000,000
AL	.FILL   b0100001001000000

BH	.fill	b0000000000010110 	; dec 1,500,000
BL	.fill	b1110001101100000

AH2	.fill	xffff
AL2	.fill	xffff
BH2	.fill	0
BL2	.fill	1

AH3	.fill	x7fff



; Function for 32 bit signed add
; R0 MSW A, R1 LSW A, R2 MSW B, R3 LSW B
; returns result in r4 and r5, overflow in r6
ADD32	st	r0, amsw	; save values	;R0 - used for unsadds
	st	r1, alsw			;R1 - used for unsadd
	st	r2, bmsw			;R2 - carry
	st	r3, blsw			;R3, R4 - used to add msw's
						;R5 - msw sum
						;R6 - sign after first add
	st	r7, reg7
	add	r0, r1, #0	; set r0 to ALSW
	add	r1, r3, #0	; set r1 to BLSW
	jsr	UNSADD		; unsigned add alsw and blsw
	st	r0, lsw
	ld	r3, amsw	; load amsw
	ld	r4, bmsw	; load bmsw
	add	r5, r3, r4	; add amsw and bmsw using regular signed add
	add	r5, r5, r1	; add carry
	and	r6, r6, #0
	and	r3, r3, r3	; check if a is positive or negative
	brzp	apADD32
	brn	anADD32

apADD32		and	r4, r4, r4	; a is positive, check sign of b
		brzp	apbpADD32
		brn	doneADD32	; no overflow

anADD32		and	r4, r4, r4	; a is negative	check sign of b
		brn	anbnADD32
		brzp	doneADD32	; no overflow

apbpADD32	and	r5, r5, r5 	; set CC for r5 (msw result)
		brzp	doneADD32	; if its positive | zero, no overflow
		add	r6, r6, #1
		br 	doneADD32

anbnADD32	and	r5, r5, r5	; set CC for r5
		brn	doneADD32	; if negative , no overflow
		add	r6, r6, #1	
		br	doneADD32

doneADD32	ld	r0, amsw
		ld	r1, alsw
		ld	r2, bmsw
		ld	r3, blsw
		add	r4, r5, #0
		ld	r5, lsw
		ld	r7, reg7
		
		ret
	
reg7	.blkw	1
	

amsw	.blkw 	1
alsw	.blkw 	1

bmsw	.blkw	1
blsw	.blkw	1

msw	.blkw   1
lsw	.blkw	1

; Function to add two unsigned 16-bit numbers.
; On input: R0 and R1 are the two numbers.
; On exit, R0 = the sum, R1 = the overflow bit (i.e. if addition overflows, R1 = 1).
UNSADD	ST R0, A	; Save registers: First number to add
	ST R1, B	; Second number to add
	ST R2, SAVER2	; Register to work in.

; We have 3 potential situations: 
; Case 1: R0 and R1 both less than or equal to 32767.
;         In this case, overflow cannot occur.
; Case 2: R0 and R1 both greater than or equal to 32768.
;	  In this case, overflow must occur.
; Case 3: One register greater than or equal to 32767, one less than or equal to 32768.
;         In this case, overflow might occur; must check further.

; Note: Since the LC-3 only deals natively with 16-bit signed numbers,
; there is an easy way to test if an unsigned number is <= 32767 or >= 32768.
; If the number is <= 32767, bit 15 will be zero, so the LC-3 will consider it positive or zero.
; If the number is >= 32768, bit 15 will be one, so the LC-3 will consider it negative.

; Let's start testing
	ADD R0,R0,#0	; Set condition codes from R0
	BRN R0BIG	; Branch if R0 >= 32768
	ADD R1,R1,#0	; Set condition codes from R1
	BRN CASE3	; Branch if R1 >= 32768

CASE1			; Both are <= 32767, so we are at Case 1.
	ADD R0,R0,R1	; Put sum in R0, ignoring the fact that it might overflow into Bit 15.
	AND R1,R1,#0	; Clear R1 (no overflow)
	BR DONE

R0BIG	ADD R1,R1,#0	; R0 is >= 32768. Now set condition codes from R1.
	BRZP CASE3	; R1 <= 32767, so we have Case 3

; Both are >= 32768, so we have Case 2.
CASE2	ADD R0,R0,R1	; Put sum in R0.
	AND R1,R1,#0
	ADD R1,R1,#1	; Put 1 into R1, indicating overflow
	BR DONE

CASE3			; One is <= 32767, the other is >= 32768
	ADD R0,R0,R1	; Put sum into R0
	AND R1,R1,#0	; Start by assuming no overflow ;FIXED - add the numbers first, then clear r1
	AND R0,R0,R0	; set condition code for r0     ; ADDED - check condition
	BRN DONE	; If sum still has bit 15 set, no overflow
	ADD R1,R1,#1	; Otherwise, overflow did occur

; All done; either fall through or branch to here
DONE	LD R2, SAVER2	; Restore working registers and return
	RET

A	.BLKW 1		; Store first number here
B	.BLKW 1		; Store second number here
SAVER2	.BLKW 1		; Save R2 here

	.END
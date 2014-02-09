; Simple multiply and divide function
	.ORIG	x3000
; test some cases of MULT
	ld	r0, ten
	ld	r1, three
	jsr	MULT
	
	ld	r0, nten
	ld	r1, three
	jsr	MULT
	
	ld	r0, nten
	ld	r1, zero
	jsr	MULT

; test some cases of DIV

	ld	r0, nten
	ld	r1, nthree
	jsr	MULT

	ld	r0, zero
	ld	r1, nthree
	jsr	MULT

	ld	r0, ten
	ld	r1, nthree
	jsr	MULT

	ld	r0, zero
	ld	r1, zero
	jsr	MULT

	ld	r0, nten
	ld	r1, three
	jsr	MULT

	ld	r0, zero
	ld	r1, three
	jsr	MULT

	ld	r0, ten
	ld	r1, three
	jsr	MULT
	
	HALT

ten	.fill 	#10
three	.fill	#3
nten	.fill	#-10
nthree	.fill	#-3
zero	.fill	#0


; Function which calls MULT1, but returns the correct multiplication result if R1 is zero or negative
MULT	st	r7, reg7	; save r7
	st	r1, reg1
	st	r0, reg0
	and	r1, r1, r1
	brzp	notneg0
	not	r1, r1
	add	r1, r1, #1

notneg0	jsr	MULT1
	ld	r1, reg0
	and	r1, r1, r1
	brzp	notneg
	not	r2, r2
	add	r2, r2, #1
	
notneg	ld	r1, reg1
	and	r1, r1, r1
	brnp	notzero	
	and	r2, r2, #0

notzero	ld	r0, reg0
	ld	r1, reg1
	ld	r7, reg7
	ret			; return
reg0	.blkw 1
reg1	.blkw 1
reg7	.blkw 1
	


; Function which multiplies R0 and R1, giving R2.
; Note: Only works for R1 strictly positive.
; Also, will not work if answer is bigger than will fit in signed 16-bit number.
MULT1	ST	R1, SAVE_R1	; Save register that will be modified
	AND	R2, R2, #0	; Start by clearing the answer

				; Every time through the following loop,
				; we add R0 to R2.
				; Then we decrement R1 until it reaches zero.
LOOP	ADD	R2, R2, R0
	ADD	R1, R1, #-1
	BRp	LOOP		; Loop if not done yet.
				; Done. R2 now equals R0 times (original) R1.

	LD	R1, SAVE_R1	; Restore register
	RET

SAVE_R1	.BLKW	1		; Save R1 here

; Function which calls DIV1, but returns the correct result for negative or zero inputs
DIV	st	r0, reg0d
	st	r1, reg1d
	st	r7, reg7d
	and	r1, r1, r1	; check sign of denomiator 
	brn	bn
	brz	bz
	brp	bp

; Cases where B is negative
bn	and	r0, r0, r0
	brn	bnan
	brz	az
	brp	bnap

; B negative, A negative
bnan	not	r0, r0		; make r0 positive
	add	r0, r0, #1	
	not 	r1, r1		; make r1 positive
	add 	r1, r1, #1
	jsr 	DIV1
	not	r3, r3		; negate the remainder
	add	r3, r3, #1	
	br 	divdone

; B negative, A positive
bnap	not	r1, r1		; make r1 positive
	add	r1, r1, #1
	jsr	DIV1
	not	r2, r2		; negate the quotient
	add	r2, r2, #1
	br 	divdone	

; Case where B is zero
bz	and	r2, r2, #0	; set r2 and r3 to 0
	and	r3, r3, #0
	br divdone	; b is zero, exit because it is division by zero

; Cases where B is negative
bp	and	r0, r0, r0	
	brn	bpan
	brz	az
	brp	bpap

; B positive, A negative
bpan	not	r0, r0		; make r0 positive
	add	r0, r0, #1	
	jsr	DIV1
	not	r2, r2		; negate quotient
	add	r2, r2, #1
	not	r3, r3		; negate remainder
	add	r3, r3, #1
	br 	divdone

; B positive, A positive
bpap	jsr	DIV1
	br	divdone
	

; A zero
az	and	r2, r2, #0	; quotient and remainder are zero
	and	r3, r3, #0
	br 	divdone

divdone ld	r0, 	reg0d	; load saved registers
	ld	r1, 	reg1d
	ld	r7,	reg7d
	ret			; return

reg0d	.blkw 1
reg1d	.blkw 1
reg7d	.blkw 1
	
	
; Function which divides R0 by R1, giving the result in R2 and the remainder in R3
; Note: Only works for R0 and R1 strictly positive.
; Also, will not work if answer is bigger than will fit in signed 16-bit number.
DIV1	ST	R1, reg1d1	; Save register that will be modified
	st	r0, reg0d1
	AND	R2, R2, #0	; Start by clearing the answer
	not	r1, r1
	add	r1, r1, #1
	

				; Every time through the following loop,
				; we add R0 to R2.
				; Then we decrement R1 until it reaches zero.
LOOPD	ADD	R2, R2, #1
	add	R0, R0, r1
	BRzp	LOOPD		; Loop if not done yet.
	not	r1, r1
	add	r1, r1, #1
	add	r0, r0, r1
	add	r2, r2, #-1
	and	r3, r0, r0
				; Done. R2 now equals R0 times (original) R1.

	ld	r0, reg0d1
	LD	R1, reg1d1	; Restore register
	RET

reg0d1	.blkw	1
reg1d1	.BLKW	1		; Save R1 here

	.END	
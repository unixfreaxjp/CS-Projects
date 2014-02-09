; Simple multiply function
	.ORIG	x3000
	LD	R0, X	; Load the two pieces of data
	LD	R1, Y
	JSR	DIV1	; Multiply R0 and R1, giving R2
	HALT

X	.FILL 10
Y	.FILL 3

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



; Function which multiplies R0 and R1, giving R2.
; Note: Only works for R1 strictly positive.
; Also, will not work if answer is bigger than will fit in signed 16-bit number.
DIV1	ST	R1, reg1d 	; Save register that will be modified
	st	r0, reg0d
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

	ld	r0, reg0d
	LD	R1, reg1d	; Restore register
	RET

reg0d	.blkw	1
reg1d	.BLKW	1		; Save R1 here

	.END	
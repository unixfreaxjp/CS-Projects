; Trap x38
; Input a character into r0, and then outputs a '*'

.orig	x700
	st	r1, reg1	; store r1, used to check the DSR for output
	st	r2, reg2	; store r2, used to store the * character

waitin	ldi	r0, KBSR	; check the keyboard status
	brzp	waitin		; loop while theres no input
	ldi	r0, KBDR	; load the character into r0

waitout	ldi	r1, DSR		; check the display status
	brzp	waitout		; loop while display is not ready
	ld	r2, star	; load '*' into r2 and store it to the display data
	sti	r2, DDR		

	ld	r1, reg1	; restore registers
	ld	r2, reg2	
	ret			; return

KBSR	.fill	xFE00
KBDR	.fill	xFE02
DSR	.fill	xFE04
DDR	.fill	xFE06
pcinc	.fill 	x0000
star	.fill	x002A
reg1	.fill	x0000
reg2	.fill	x0000
.end
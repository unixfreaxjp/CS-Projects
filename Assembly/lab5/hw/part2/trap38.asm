; Trap x38
; Input a character into r0, and then outputs a '*'

.orig	x700
	st	r1, reg1	; store r1, used to check the DSR for output
	st	r2, reg2	; store r2, used to store the * character
	st	r3, reg3	; used to check if the character is printable

waitin	ldi	r0, KBSR	; check the keyboard status
	brzp	waitin		; loop while theres no input
	ldi	r0, KBDR	; load the character into r0

; check if the character is in the range of printable characters
	ld	r3, minprint	; load the min printable character
	not	r3, r3		; negate it for comparison
	add	r3, r3, #1
	add	r3, r3, r0	; add the character to the negative value
	brn	done		; if the character is less than the min, skip output (not printable)
	
	ld	r3, maxprint	; load the max printable character
	not	r3, r3		; negate it for comparison
	add	r3, r3, #1
	add	r3, r3, r0	; add the character to the negative value
	brp	done		; if the character is greater than the max, skip output	(not printable)
	
waitout	ldi	r1, DSR		; check the display status
	brzp	waitout		; loop while display is not ready
	ld	r2, star	; load '*' into r2 and store it to the display data
	sti	r2, DDR		

done	ld	r1, reg1	; restore registers
	ld	r2, reg2	
	ld	r3, reg3
	ret			; return

KBSR	.fill	xFE00
KBDR	.fill	xFE02
DSR	.fill	xFE04
DDR	.fill	xFE06
pcinc	.fill 	x0000
star	.fill	x002A
minprint .fill x0020
maxprint .fill x007E
reg1	.fill	x0000
reg2	.fill	x0000
reg3	.fill 	x0000
.end
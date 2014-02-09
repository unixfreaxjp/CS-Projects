				; Program to copy an array of data from one block of memory to another.
				; Add decimal 14 to the value of each word before storing in the destination array.
				; Keep copying until the program reaches a zero value in the source array.

	.ORIG	x3000		; Program starts at address hex 3000
	LD	R0, SRC		; Load SRC into Register 0 (changed from LEA to LD)
	LD	R1, DEST	; Load DEST into Register 1
LOOP	LDR	R2, R0, #0	; Load contents of memory location whose address is in R0, into R2
	BRZ	DONE		; If last operation was zero, branch to DONE (changed from SRC to DONE)
	ADD	R2, R2, #14	; Add decimal 14 to value in R2
	STR	R2, R1, #0	; Store contents of R2 into memory whose address is in R1
	ADD	R0, R0, #1	; Add 1 to R0
	ADD	R1, R1, #1	; Add 1 to R1
	BR	LOOP		; branch to LOOP (changed from BRN to BR)
DONE	TRAP	x25		; Trap to HALT the program.
SRC	.FILL	x4000		; Address of start of source data array
DEST	.FILL	x5000		; Address of start of destination array.
	.END

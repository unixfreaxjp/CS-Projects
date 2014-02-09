; This program determines whether or not a string is a palindrome.
; The user loads the string into memory beginning at location x5000
; (the string is in .STRINGZ format)
; Upon completion, R5 contains 1 if the string is a palindrome,
; and 0 if the string is not a palindrome.

	.ORIG x3000

	LD	R0, PTR
	ADD	R1, R0, #0

; (put a comment here explaining the purpose of the 4-word loop
;  that begins at AGAIN)

AGAIN	LDR	R2, R1, #0
	BRz	CONT
	ADD	R1, R1, #1
	BRnzp	AGAIN

CONT    ------------------ (a)
LOOP	LDR	R3, R0, #0
	------------------ (b)

; (put a comment here explaining the purpose of the next four instructions)
	NOT	R4, R4
	ADD	R4, R4, #1
	ADD	R3, R3, R4
	BRnp	NO
	------------------ (c)
	------------------ (d)

; (put a comment here explaining the purpose of the next four instructions)
	NOT	R2, R0
	ADD	R2, R2, #1
	ADD	R2, R1, R2
	BRnz	YES
	------------------ (e)

; The string is a palindrome, so set R5 to 1
YES	AND	R5, R5, #0
	ADD	R5, R5, #1
	BRnzp	DONE

; The string is not a palindrome, so set R5 to 0
NO	AND	R5, R5, #0

DONE	HALT

PTR	.FILL   x5000		; the address of the string
	.END

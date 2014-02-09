; This program determines whether or not a string is a palindrome.
; The user loads the string into memory beginning at location x5000
; (the string is in .STRINGZ format)
; Upon completion, R5 contains 1 if the string is a palindrome,
; and 0 if the string is not a palindrome.

	.ORIG x3000

	LD	R0, PTR
	ADD	R1, R0, #0

; finds the address of the last character
; address is stored in R1

AGAIN	LDR	R2, R1, #0
	BRz	CONT
	ADD	R1, R1, #1
	BRnzp	AGAIN

CONT    ADD	R1, R1, #-1; (a) subract one from r1 so it points to the last character
LOOP	LDR	R3, R0, #0
	LDR	R4, R1, #0 ;(b)

; checks if the first and second characters are equal. If they aren't, branch to NO
; otherwise advance the ptrs by one (towards each other)
	NOT	R4, R4
	ADD	R4, R4, #1
	ADD	R3, R3, R4
	BRnp	NO
	ADD	R0, R0, #1 ;(c) increment first ptr
	ADD	R1, R1, #-1 ;(d) decrement second ptr

; if the pointer to the first letter is equal to or past the second, then the word is a palindrome
; (all letters have been checked). Branches to YES
	NOT	R2, R0
	ADD	R2, R2, #1
	ADD	R2, R1, R2
	BRnz	YES
	BR LOOP; (e) run the loop again for the next pair of characters

; The string is a palindrome, so set R5 to 1
YES	AND	R5, R5, #0
	ADD	R5, R5, #1
	BRnzp	DONE

; The string is not a palindrome, so set R5 to 0
NO	AND	R5, R5, #0

DONE	HALT

PTR	.FILL   x5000		; the address of the string
	.END

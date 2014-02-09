
.orig	x3000

MAIN		and	r2, r2, #0	; R2 array address
		and	r3, r3, #0	; R3 array length
		ld	r2, arrayloc
		
		
inputMAIN	lea	r0, prompt	; print prompt
		puts
		lea	r0, numstring	; input a number string
		ld	r1, len
		jsr	READNUM
		and	r1, r1, r1	; if the length is zero, sort the list
		brz	dosortMAIN

		jsr	STRtoDECs	; convert from ascii to decimal (signed)
		str	r1, r2, #0	; save number in the array
		add	r2, r2, #1	; increment array pointer
		add	r3, r3, #1	; increment array length
		
		br	inputMAIN	; loop

dosortMAIN	ld	r0, arrayloc	; load address into r0
		add	r1, r3, #0	; load length into r1
		jsr	SORT
		
		lea	r0, message
		puts	
		
		lea 	r1, numstring	; load string pointer
		ld	r4, arrayloc	; load array pointer
printnumsMAIN	ldr	r0, r4, #0
		jsr	DECtoSTR	; convert from decimal to ascii
		
		lea	r0, numstring	; print number
		puts
		ld	r0, LF		; print newline
		out

		add	r4, r4, #1	; increment pointer
		add	r3, r3, #-1	; decrement length counter
		brp 	printnumsMAIN	; loop while length > 0				
		halt	

prompt		.stringz	"Enter a number: "
message		.stringz	"Array has been sorted:\n"
testnum 	.fill		#0
len		.fill		7
numstring	.blkw		7	

; The actual array is at the end of the program so that it can have an indefinite length
arrayloc	.fill	array


; Inputs a number from the console, and stores it to a given address
; args: R0, address of the string, R1, maximum length
; returns R1, length of the string
READNUM		st	r0, r0READNUM	; used by getc & out traps
		st	r2, r2READNUM	; check for LF & CR
		st	r3, r3READNUM	; track string length
		st	r4, r4READNUM	; starting address
		st	r5, r5READNUM	; max address
		st	r6, r6READNUM	; used to check if we are at the end
		st	r7, r7READNUM
			
		add	r4, r0, #0	; store start address in r4
		add	r5, r4, r1	; get end address (start+len-1)
		add	r5, r5, #-1
		not	r5, r5		; negate max address
		add	r5, r5, #1	
		and	r1, r1, #0	; set string length to zero	

loopREADNUM	getc	; read a character	

		ld	r2, LF		; load line feed
		not	r2, r2		; negate
		add	r2, r2, #1
		add	r2, r2, r0	; check if character is line feed
		brz	doneREADNUM

		ld	r2, CR 		; load carriage return
		not	r2, r2		; negate
		add	r2, r2, #1
		add	r2, r2, r0	; check if character is carriage return
		brz	doneREADNUM

		str	r0, r4, #0	; store character
		add	r4, r4, #1	; increment address
		add	r1, r1, #1	; increment length

		add	r6, r5, r4	; check string length
		brz	doneREADNUM	

		out			; print out the character
		br	loopREADNUM		

doneREADNUM	ld	r0, LF		; load newline
		out			; print newline
		and	r0, r0, #0	; set r0 to 0
		str	r0, r4, #0	; write 0 at the end

		ld	r0, r0READNUM	; restore registers
		ld	r2, r2READNUM	
		ld	r3, r3READNUM	
		ld	r4, r4READNUM	
		ld	r5, r5READNUM	
		ld	r6, r6READNUM	
		ld	r7, r7READNUM	
		ret			; return

r0READNUM	.blkw	1
r2READNUM	.blkw	1
r3READNUM	.blkw	1
r4READNUM	.blkw	1
r5READNUM	.blkw	1
r6READNUM	.blkw	1
r7READNUM	.blkw	1

LF	.fill	x0A
CR	.fill	x0D

; Converts from a decimal number to a ASCII string
; args: R0, the number , R1, pointer to the string
; returns nothing
DECtoSTR	st	r0, r0DECtoSTR	; R0 contains the number
		st	r1, r1DECtoSTR	; contains the 10s place value
		st	r2, r2DECtoSTR	; holds the division/multiplication result, temp
		st	r3, r3DECtoSTR	; holds the modulus result, temp
		st	r4, r4DECtoSTR	; holds a pointer to the string
		st	r5, r5DECtoSTR	; holds the number of digits
		st	r6, r6DECtoSTR	; used to find the ascii value of a digit
		st	r7, r7DECtoSTR	
		st	r0, divnumDECtoSTR
		
		and	r0, r0, r0	; if the number = 0, put in zero and return
		brnp	nonzeroDECtoSTR
		ld	r6, ASCII0	; load '0', put it in the string
		str	r6, r1, #0
		and	r6, r6, #0	; set r6 to zero and store as null-terminator
		str	r6, r1, #1
		br	doneDECtoSTR		

nonzeroDECtoSTR	add	r4, r1, #0	; set r4 to the string address
		and	r1, r1, #0	; set r1 to 10
		add	r1, r1, #10
		
		and	r0, r0, r0
		brzp	digitsDECtoSTR
		ld	r2, CHAR_MINUS	; load the '-' character
		str	r2, r4, #0	; store it into the first position
		add	r4, r4, #1	; increment pointer
		not	r0, r0		; make the number positive
		add	r0, r0, #1
		st	r0, divnumDECtoSTR	; save new number			
		
digitsDECtoSTR	and	r5, r5, #0	; set r3 to 0
dloopDECtoSTR	and	r0, r0, r0	; check if the number = 0
		brz	strDECtoSTR	
		jsr	DIV		; divide num by 10 
		add	r0, r2, #0	; set r0 to r2 (quotient)
		add	r5, r5, #1	; increment r5
		br	dloopDECtoSTR

strDECtoSTR	add	r5, r5, r4	; get end address
		str 	r0, r5, #0	; store as null-terminator (R0 = 0)
		add	r5, r5, #-1	; decrement
		ld	r0, divnumDECtoSTR  ; restore number (positive)
sloopDECtoSTR	jsr	DIV
		ld	r6, ASCII0
		add	r6, r6, r3	; get the ascii digit
		str	r6, r5, #0	; store it in the next location
		add	r5, r5, #-1	; decrement address
		add	r0, r2, #0	; set r0 to the quotient
		brp	sloopDECtoSTR	; loop while the quotient > 0		

doneDECtoSTR	ld	r0, r0DECtoSTR	; restore registers
		ld	r1, r1DECtoSTR	
		ld	r2, r2DECtoSTR	
		ld	r3, r3DECtoSTR	
		ld	r4, r4DECtoSTR	
		ld	r5, r5DECtoSTR
		ld	r6, r6DECtoSTR
		ld	r7, r7DECtoSTR
		ret			; return

r0DECtoSTR	.blkw	1
r1DECtoSTR	.blkw	1
r2DECtoSTR	.blkw	1
r3DECtoSTR	.blkw	1
r4DECtoSTR	.blkw	1
r5DECtoSTR	.blkw	1
r6DECtoSTR	.blkw   1
r7DECtoSTR	.blkw	1
divnumDECtoSTR	.blkw	1
CHAR_MINUS	.fill 	#45
ASCII0		.fill	#48


; Calls STRtoDEC accounting for the sign at the beginning
; args:	R0, starting address of the number
; returns the value in R1
STRtoDECs	st	r0, r0STRtoDECs	; store registers
		st	r2, r2STRtoDECs
		st	r3, r3STRtoDECs
		st	r4, r4STRtoDECs
		st	r7, r7STRtoDECs

		and	r4, r4, #0	; clear R4
		ldr	r2, r0, #0	; load first character
		ld	r3,  MINUS	; is it '-'?
		add	r3, r3, r2
		brnp	plusSTRtoDECs
		add	r4, r4, #-1
		br	convSTRtoDECs
plusSTRtoDECs	ld	r3, PLUS	; is it '+'?
		add	r3, r3, r2
		brnp	convSTRtoDECs
		add	r4, r4, #1	; store the sign

convSTRtoDECs	and	r4, r4, r4
		brz	callSTRtoDECs
		add	r0, r0, #1	; since there is a sign character, increment the start address
callSTRtoDECs	jsr	STRtoDEC
		and	r4, r4, r4
		brzp	doneSTRtoDECs	; check if the sign in negative
		not	r1, r1		; negate r1
		add	r1, r1, #1

doneSTRtoDECs	ld	r0, r0STRtoDECs	; restore registers
		ld	r2, r2STRtoDECs
		ld	r3, r3STRtoDECs
		ld	r4, r4STRtoDECs
		ld	r7, r7STRtoDECs
		ret			; ret
		

r0STRtoDECs	.blkw	1
r2STRtoDECs	.blkw	1
r3STRtoDECs	.blkw	1
r4STRtoDECs	.blkw	1
r7STRtoDECs	.blkw	1

MINUS	.fill	#-45
PLUS	.fill	#-43

; Converts from null-terminated string to a decimal
; args:	R0, starting address of the number
; returns the value in R1

STRtoDEC	st	r0, reg0STRtoDEC	; R0, need to call MULT, used for comparisons
		st	r1, reg1STRtoDEC	; R1, used to store the tens place value
		st	r2, reg2STRtoDEC	; R2, used to temporarily store values (when multiplying, loading)
		st	r5, reg5STRtoDEC	; R5, used to count the length of the string and the current character
		st	r3, reg3STRtoDEC	; R3, used to store the current character
		st	r4, reg4STRtoDEC	; R4, negative of the starting address
		st	r6, reg6STRtoDEC	; R6 used to store the total
		st	r7, reg7STRtoDEC

; count the length of the string
		not	r4, r0		; set r4 to negative start
		add	r4, r4, #1		
		and	r5, r0, r0	; set r2 as the starting address

; find the address of the last character in the string		
countSTRtoDEC	ldr	r3, r5, #0	; load the character
		ld	r0, ASCII_MINn	; load - ascii 0
		add	r0, r0, r3
		brn	convertSTRtoDEC ; character is not a number (< '0') ; proceed with converting
		ld	r0, ASCII_MAXn	; load - ascii 9
		add	r0, r0, r3
		brp	convertSTRtoDEC	; character is not a number (> '9') ; proceed with converting
		add	r5, r5, #1	
		br	countSTRtoDEC	

; starting with the ending address, moves backwards adding each digit's value to the total until the start is reached
convertSTRtoDEC	and	r6, r6, #0		; clear r6 (total)
		and	r1, r1, #0 		; set r1 to 1
		add	r1, r1, #1
		add	r5, r5, #-1
loopSTRtoDEC	and	r0, r4, r4		; if we have reached the start, we're done
		add	r0, r5, r0
		brn	doneSTRtoDEC
		ldr	r3, r5, #0		; load character into r3
		ld	r2, ASCII_MINn
		add	r3, r3, r2		; get decimal value of digit
		and	r0, r3, r3		; load digit into r0
		jsr 	MULT			; r2 = r1*r0
		add	r6, r6, r2		; add to total
		add	r5, r5, #-1		; decrement address
		
		and	r0, r0, #0		; multiple place value by 10
		add	r0, r0, #10		
		JSR	MULT			
		and	r1, r2, r2
		br	loopSTRtoDEC		

doneSTRtoDEC	and	r1, r6, r6
		ld	r2, reg2STRtoDEC	; restore registers
		ld	r3, reg3STRtoDEC
		ld	r4, reg4STRtoDEC
		ld	r5, reg5STRtoDEC
		ld	r6, reg6STRtoDEC
		ld	r7, reg7STRtoDEC
		ret				; return	

reg0STRtoDEC	.blkw	1
reg1STRtoDEC	.blkw	1
reg2STRtoDEC	.blkw	1
reg3STRtoDEC	.blkw	1
reg4STRtoDEC	.blkw	1
reg5STRtoDEC	.blkw 	1
reg6STRtoDEC	.blkw	1
reg7STRtoDEC	.blkw 	1

ASCII_MINn	.fill	#-48	; ascii '0', negated for easier use
ASCII_MAXn	.fill	#-57	; ascii '9', negated for easier use



; Function which calls MULT1, but returns the correct multiplication result if R1 is zero or negative
MULT	st	r7, reg7MULT	; save r7
	st	r1, reg1MULT
	st	r0, reg0MULT
	and	r1, r1, r1
	brzp	notneg0MULT
	not	r1, r1
	add	r1, r1, #1

notneg0MULT	jsr	MULT1
		ld	r1, reg0MULT
		and	r1, r1, r1
		brzp	notnegMULT
		not	r2, r2
		add	r2, r2, #1
	
notnegMULT	ld	r1, reg1MULT
		and	r1, r1, r1
		brnp	notzeroMULT	
		and	r2, r2, #0

notzeroMULT	ld	r0, reg0MULT
		ld	r1, reg1MULT
		ld	r7, reg7MULT
		ret			; return
reg0MULT	.blkw 1
reg1MULT	.blkw 1
reg7MULT	.blkw 1
	


; Function which multiplies R0 and R1, giving R2.
; Note: Only works for R1 strictly positive.
; Also, will not work if answer is bigger than will fit in signed 16-bit number.
MULT1	st	r1, reg0MULT1	; Save register that will be modified
	and	r2, r2, #0	; Start by clearing the answer

				; Every time through the following loop,
				; we add R0 to R2.
				; Then we decrement R1 until it reaches zero.
loopMULT1 	add	r2, r2, r0
	add	r1, r1, #-1
	brp	loopMULT1	; Loop if not done yet.
				; Done. R2 now equals R0 times (original) R1.

	ld	r1, reg0MULT1	; Restore register
	ret

reg0MULT1	.blkw	1	; Save R1 here

; Function which calls DIV1, but returns the correct result for negative or zero inputs
DIV	st	r0, reg0DIV
	st	r1, reg1DIV
	st	r7, reg7DIV
	and	r1, r1, r1	; check sign of denomiator 
	brn	bnDIV
	brz	bzDIV
	brp	bpDIV

; Cases where B is negative
bnDIV	and	r0, r0, r0
	brn	bnanDIV
	brz	azDIV
	brp	bnapDIV

; B negative, A negative
bnanDIV	not	r0, r0		; make r0 positive
	add	r0, r0, #1	
	not 	r1, r1		; make r1 positive
	add 	r1, r1, #1
	jsr 	DIV1
	not	r3, r3		; negate the remainder
	add	r3, r3, #1	
	br 	doneDIV

; B negative, A positive
bnapDIV	not	r1, r1		; make r1 positive
	add	r1, r1, #1
	jsr	DIV1
	not	r2, r2		; negate the quotient
	add	r2, r2, #1
	br 	doneDIV	

; Case where B is zero
bzDIV	and	r2, r2, #0	; set r2 and r3 to 0
	and	r3, r3, #0
	br 	doneDIV	; b is zero, exit because it is division by zero

; Cases where B is negative
bpDIV	and	r0, r0, r0	
	brn	bpanDIV
	brz	azDIV
	brp	bpapDIV

; B positive, A negative
bpanDIV	not	r0, r0		; make r0 positive
	add	r0, r0, #1	
	jsr	DIV1
	not	r2, r2		; negate quotient
	add	r2, r2, #1
	not	r3, r3		; negate remainder
	add	r3, r3, #1
	br 	doneDIV

; B positive, A positive
bpapDIV	jsr	DIV1
	br	doneDIV
	

; A zero
azDIV	and	r2, r2, #0	; quotient and remainder are zero
	and	r3, r3, #0
	br 	doneDIV

doneDIV ld	r0, 	reg0DIV	; load saved registers
	ld	r1, 	reg1DIV
	ld	r7,	reg7DIV
	ret			; return

reg0DIV	.blkw 1
reg1DIV	.blkw 1
reg7DIV	.blkw 1
	
	
; Function which divides R0 by R1, giving the result in R2 and the remainder in R3
; Note: Only works for R0 and R1 strictly positive.
; Also, will not work if answer is bigger than will fit in signed 16-bit number.
DIV1	st	r0, reg0DIV1	; Save register that will be modified
	st	r1, reg1DIV1	
	
	and	r2, r2, #0	; Start by clearing the answer
	not	r1, r1
	add	r1, r1, #1

loopDIV1	add	r2, r2, #1
	add	r0, r0, r1
	brzp	loopDIV1		; Loop if not done yet.
	not	r1, r1
	add	r1, r1, #1
	add	r0, r0, r1
	add	r2, r2, #-1
	and	r3, r0, r0
				; Done. R2 now equals R0 times (original) R1.

	ld	r0, reg0DIV1
	ld	r1, reg1DIV1	; Restore register
	ret

reg0DIV1	.blkw	1
reg1DIV1	.blkw	1		; Save R1 here


; SORTING FUNCTION - Sorts a list of numbers
; Arguments: R0 is the start of the number array, R1 is the array's length
; Uses a simple selection sort to sort the list in descending order
; The function goes through each element in the list, swapping it with the largest element that appears after it.

SORT	st	r0, reg0SORT	; Save all registers, R0 stores the starting index of the outer loop
	st	r1, reg1SORT	; R1 contains the negative of the end address, used for checking loop conditions
	st	r2, reg2SORT	; R2 stores the max number in the array
	st	r3, reg3SORT	; R3 stores the address of the max number in the array
	st	r4, reg4SORT	; R4 stores the starting index of the inner loop
	st	r5, reg5SORT	; R5 used to load and compare the current max and the next number 
	st	r6, reg6SORT	; R6 used to compare current and end address for loops
	st	r7, reg7SORT	; R7 temporarily stores the starting number

	add	r1, r1, r0 	; get the end address and make it negative for easy loop conditions
	add	r1, r1, #-1	; end address is start+len-1
	not	r1, r1		; negate
	add 	r1, r1, #1	

loopASORT	ldr	r7, r0, #0	; save the starting number
	and	r2, r7, r7	; initialize the max to the starting number
	and	r3, r0, r0	; initialize the max address to the starting address
	
	and	r4, r0, r0	; inner loop starts at the number right after the starting number
loopBSORT	add	r4, r4, #1	; increment loop counter
	ldr	r5, r4, #0	; load next number
	not	r5, r5		; negate so the current max and the next number can be compared
	add	r5, r5, #1
	add	r5, r2, r5	; add the max
	brzp	nextjSORT	; if the max is greater than or equal to the current number, dont save it
	ldr	r2, r4, #0	; save the new max
	and	r3, r4, r4	; save the new max addr

nextjSORT	add	r6, r4, r1	; loop if we are not at the end address yet
	brn	loopBSORT
	
	; swap the two numbers. If no max was found, this has no effect	
	str	r2, r0, #0	; store max into the starting number's address
	str	r7, r3, #0	; store starting number into the max's address
  
nextiSORT	add	r6, r0, r1	; loop if we are not at the end address yet
	add	r6, r6, #1	; there no point if running the loop for the last number in the array
	brzp	finSORT		; so we end the loop 1 element before the end by going to the end (fin)
	add	r0, r0, #1	; increment the starting address 
	br	loopASORT		; loop 
	
finSORT	ld	r0, reg0SORT	; restore all registers
	ld	r1, reg1SORT
	ld	r2, reg2SORT
	ld	r3, reg3SORT
	ld	r4, reg4SORT
	ld	r5, reg5SORT
	ld	r6, reg6SORT
	ld	r7, reg7SORT
	ret			; return

; Register stores
reg0SORT	.fill	x0000
reg1SORT	.fill	x0000
reg2SORT	.fill	x0000
reg3SORT	.fill	x0000
reg4SORT	.fill	x0000
reg5SORT	.fill	x0000
reg6SORT	.fill	x0000
reg7SORT	.fill	x0000

;number  array is at the very end of the program (infinite length)
array	.fill x0000

.end
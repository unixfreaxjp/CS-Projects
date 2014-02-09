	.orig	x3000
; MAIN - calls the sorting function to sort an array of numbers
	lea	r0, array	; load the address of the array
	ld	r1, size	; load the size of the array
	jsr	SORT

	; sort another array to show that the subroutine is serially reentrant
	lea	r0, array2
	ld	r1, size2	
	jsr	SORT		; call the sorting function

	halt			; end of program

; number array to sort

size	.fill 	#20	; size of the array
array	.fill 	#-26	; the array
	.fill 	#33
	.fill 	#-59
	.fill 	#-17
	.fill 	#-0
	.fill 	#1
	.fill 	#-1
	.fill 	#32
	.fill 	#235
	.fill 	#-49
	.fill 	#7
	.fill 	#-98
	.fill 	#9
	.fill 	#576
	.fill 	#23
	.fill 	#-300
	.fill 	#42
	.fill 	#314
	.fill 	#315
	.fill 	#-7

size2	.fill 	#20	; size of the array
array2	.fill 	#-10	; the array
	.fill 	#-9
	.fill 	#-8
	.fill 	#-7
	.fill 	#-6
	.fill 	#-5
	.fill 	#-4
	.fill 	#-3
	.fill 	#-2
	.fill 	#-1
	.fill 	#0
	.fill 	#1
	.fill 	#2
	.fill 	#3
	.fill 	#4
	.fill 	#5
	.fill 	#6
	.fill 	#7
	.fill 	#8
	.fill 	#9


; SORTING FUNCTION - Sorts a list of numbers
; Arguments: R0 is the start of the number array, R1 is the array's length
; Uses a simple selection sort to sort the list in descending order
; The function goes through each element in the list, swapping it with the largest element that appears after it.

SORT	st	r0, reg0	; Save all registers, R0 stores the starting index of the outer loop
	st	r1, reg1	; R1 contains the negative of the end address, used for checking loop conditions
	st	r2, reg2	; R2 stores the max number in the array
	st	r3, reg3	; R3 stores the address of the max number in the array
	st	r4, reg4	; R4 stores the starting index of the inner loop
	st	r5, reg5	; R5 used to load and compare the current max and the next number 
	st	r6, reg6	; R6 used to compare current and end address for loops
	st	r7, reg7	; R7 temporarily stores the starting number

	add	r1, r1, r0 	; get the end address and make it negative for easy loop conditions
	add	r1, r1, #-1	; end address is start+len-1
	not	r1, r1		; negate
	add 	r1, r1, #1	

loopA	ldr	r7, r0, #0	; save the starting number
	and	r2, r7, r7	; initialize the max to the starting number
	and	r3, r0, r0	; initialize the max address to the starting address
	
	and	r4, r0, r0	; inner loop starts at the number right after the starting number
loopB	add	r4, r4, #1	; increment loop counter
	ldr	r5, r4, #0	; load next number
	not	r5, r5		; negate so the current max and the next number can be compared
	add	r5, r5, #1
	add	r5, r2, r5	; add the max
	brzp	nextj		; if the max is greater than or equal to the current number, dont save it
	ldr	r2, r4, #0	; save the new max
	and	r3, r4, r4	; save the new max addr

nextj	add	r6, r4, r1	; loop if we are not at the end address yet
	brn	loopB
	
	; swap the two numbers. If no max was found, this has no effect	
	str	r2, r0, #0	; store max into the starting number's address
	str	r7, r3, #0	; store starting number into the max's address
  
nexti	add	r6, r0, r1	; loop if we are not at the end address yet
	add	r6, r6, #1	; there no point if running the loop for the last number in the array
	brzp	fin		; so we end the loop 1 element before the end by going to the end (fin)
	add	r0, r0, #1	; increment the starting address 
	br	loopA		; loop 
	
fin	ld	r0, reg0	; restore all registers
	ld	r1, reg1
	ld	r2, reg2
	ld	r3, reg3
	ld	r4, reg4
	ld	r5, reg5
	ld	r6, reg6
	ld	r7, reg7
	ret			; return

; Register stores
reg0	.fill	x0000
reg1	.fill	x0000
reg2	.fill	x0000
reg3	.fill	x0000
reg4	.fill	x0000
reg5	.fill	x0000
reg6	.fill	x0000
reg7	.fill	x0000

.end
	
	
	

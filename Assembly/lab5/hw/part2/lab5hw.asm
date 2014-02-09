
.orig x3000

MAIN	lea	r0, str0	; load the address of the string
	ld	r1, maxlen	; load the max length of the string
	jsr	STRIN
	ld	r0, endl	; print newline
	out
	lea	r0, str0	; print the string
	puts			
	ld	r0, endl	; print newline
	out
	halt

endl	.fill	x0A
maxlen	.fill	x14	; max length is 20
str0	.blkw	x15	; length is 21, so theres a place for the null-terminator
; args
; r0 the starting address
; r1 the max number of characters

;returns
; r1 number of characters stored

; registers
; r1 used to count the length of the string
; r3 used to track current memory address
; r4 stores negative of max memory address
; r5 used to check if the current address is at the end
; r6 used to store a null-terminator at the end of the string
; r7 used to check if the character is an end-of-line

STRIN	st	r0, reg0	; used by trap38 to retrieve characters
	st	r3, reg3	; r3 used to track current memory address
	st	r4, reg4	; r4 stores negative of max memory address
	st	r5, reg5	; r5 used to check if the current address is at the end
	st	r6, reg6	; r6 used to store a null-terminator at the end of the string
	st	r7, reg7	; r7 used to check if the character is an end-of-line

	and	r3, r0, r0	; store current address in r3
	add	r4, r0, r1	; determine ending address (start + len - 1)
	add	r4, r4, #1	
	not	r4, r4		; negate end address to use for comparisons
	add	r4, r4, #1
	and	r1, r1, #0	; set length counter to 0		

loop	trap x38

	ld	r7, LF		; load line feed
	not	r7, r7		; negate to compare with input char
	add	r7, r7, #1
	add	r7, r0, r7
	brz 	done		; it is a line feed, so we are done

	ld	r7, CR		; load carriage return
	not	r7, r7		; negate to compare with input char
	add	r7, r7, #1
	add	r7, r0, r7
	brz 	done		; it is a carriage return, so we are done

	str	r0, r3, #0	; store the character
	add	r3, r3, #1	; increment the character address
	add	r1, r1, #1	; increment the length counter
	
	add	r5, r3, r4	; add the negative max addr to the current addr 
	brz	done        	; if we are at the max, we are done
	br 	loop

done	and	r6, r6, #0	; set r6 to 0
	str	r6, r3, #0	; store 0 at the end of the string

	ld	r0, reg0	;restore registers
	ld	r2, reg2	
	ld	r3, reg3
	ld	r4, reg4
	ld	r5, reg5
	ld	r6, reg6
	ld	r7, reg7
	ret			; return
	

LF	.fill	x0A
CR	.fill	x0D
reg0	.fill	x0000
reg2	.fill	x0000
reg3	.fill	x0000
reg4	.fill	x0000
reg5	.fill	x0000
reg6	.fill	x0000
reg7	.fill	x0000

.end
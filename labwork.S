  # labwork.S
  # Written 2015-2017 by F Lundevall
  # Skeleton file for IS1200/IS1500 lab 1.
  # The macros PUSH and POP are in the public domain.
  # Please add your own code at the end of the file.

  #
  # Please keep the two macros PUSH and POP unchanged
  #
.macro	PUSH reg
	addi	$sp,$sp,-4
	sw	\reg,0($sp)
.endm

.macro	POP reg
	lw	\reg,0($sp)
	addi	$sp,$sp,4
.endm
  #
  # Please add your own code below this line
  #
.data
.align 2
Delay: .word 4500000
.global time2string
.global delay
.global hexasc
.global zerotonine
.text



hexasc:
	li	$t0, 0x9
	slt	$t0, $t0, $a0
	beq	$t0, $0, zerotonine
	nop
	addi	$a0, $a0, -10
	addi	$v0, $a0, 0x41
	jr	$ra
	nop


zerotonine:
	addi	$v0, $a0, 0x30
	jr	$ra
	nop


delay:
	PUSH	$ra
	PUSH	$a0

	move	$t0, $0		# i = 0


while:
	slt	$t1, $zero, $a0
	beq	$t1, $zero, exit #if t1 = a0 = 0 --> exit
	nop
	addi	$a0, $a0, -1
	move	$t1, $0
	lw	$t1, Delay


loop:
	beq	$t0, $t1, while #when i = 4711 --> branch
	nop
	addi	$t0, $t0, 1
	j	loop
	nop


exit:
	POP	$a0
	POP	$ra
	jr 	$ra
	nop


time2string:
	PUSH $ra
	PUSH $a1
	PUSH $a0

	addi	$t0, $a1, 0
	andi	$t0, $t0, 0xf
	addi	$a0, $t0, 0	#set argument
	jal	hexasc
	nop

	POP $a0

	sb	$v0, 4($a0)	# ASCII is not affected by Endianness.
				# For the string "abcd", the "a" is stored "first".
				# If you dump character data from memory, it reads from left-to-right in the dump
				# So: no matter what the actual Endianness of the hardware, single-byte character data byte order resembles Big-Endian byte order - the left-most byte goes first

	PUSH $a0

	addi	$t0, $a1, 0
	andi	$t0, $t0, 0xf0
	srl	$t0, $t0, 4	# shift because otherwise hexasc won't branch appropriately (e.g. 0x50 > 0x9 but should be seen as less)
	addi	$a0, $t0, 0	# set argument
	jal	hexasc
	nop

	POP $a0

	sb	$v0, 3($a0)
	addi	$t0, $t0, 0x3A
	sb	$t0, 2($a0)

	PUSH $a0

	addi	$t0, $a1, 0
	andi	$t0, $t0, 0xf00
	srl	$t0, $t0, 8
	addi	$a0, $t0, 0	#set argument
	jal	hexasc
	nop

	POP $a0

	sb	$v0, 1($a0)

	PUSH $a0

	addi	$t0, $a1, 0
	andi	$t0, $t0, 0xf000
	srl	$t0, $t0, 12
	addi	$a0, $t0, 0	#set argument
	jal	hexasc
	nop

	POP $a0

	sb	$v0, 0($a0)
	addi	$t0, $t0, 0x00
	sb	$t0, 5($a0)

	POP $a1
	POP $ra

	jr 	$ra
	nop

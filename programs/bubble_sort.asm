|arr 10 7 8 9 54 35 5 4 3 2 1

main:
	li $a3, 10

bubblesort:
	li $a0, 0
	li $a1, 1
	li $a2, 0
	j pass

pass:
	beq $a1, $a3, 2
	j swap
	beq $a2, $zero, 2
	j bubblesort
	exit

swap:
	lw $t0, 0($a0)
	lw $t1, 0($a1)
	blt $t0, $t1, 4
	sw $t0, 0($a1)
	sw $t1, 0($a0)
	li $a2, 1
	addi $a0, $a0, 1
	addi $a1, $a1, 1
	j pass



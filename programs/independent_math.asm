.n 10000

main:
	li $a0, 0
	li $a1, 0
	la $a2, n
	lw $a2, $zero($a2)
	mv $t0, $a2
	mv $t1, $a2
	add $t0, $t0, $a0
	add $t1, $t1, $a1
	addi $a0, $a0, 1
	addi $a1, $a1, 1
	blt $a1, $a2, -4
	add $t1, $t1, $t0
	mv $s0, $t1
	exit
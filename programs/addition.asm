.a 2
.b 3
.c 0

main:
	la $t0, b
	lw $t1, $zero($t0)
	la $t2, a
	lw $t3, $zero($t2)
	add $t4, $t1, $t3
	la $t5, c
	sw $t4, $zero($t5)
	mv $s0, $t4
	exit
	
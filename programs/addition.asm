.a 2
.b 3
.c 0


main:
	la $t0, b
	lw $t1, 0($t0)
	la $t2, a
	lw $t3, 0($t2)
	add $t4, $t1, $t3
	la $t5, c
	sw $t4, 0($t5)
	exit
	
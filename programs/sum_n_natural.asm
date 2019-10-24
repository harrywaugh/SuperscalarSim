main:
	li $t0, 1
	li $t1, 0
	li $t2, 10
	j for
for:
	beq $t0, $t2, end
	add $t1, $t1, $t0
	addi, $t0, $t0, 1
	j for
end:
	sw $t1, 0(0)
	exit

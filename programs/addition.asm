main:
# asdasd
	li $t2, 3
	li $t3, 4
	add $t4, $t2, $t3
	sub $t5, $t2, $t3
	sw $t5, Z
	li $v0, 10
	syscall
for:
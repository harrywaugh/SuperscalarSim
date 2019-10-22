.variable 10

main:
	li $t2, 3
	li $t3, 4
	add $t4, $t2, $t3
	sub $t5, $t3, $t2
	add $t6, $t5, $t4
	sw $t6, 0(50)
	exit
	
for:
.n 12

main:
	la $t0, n
	lw $a0, $zero($t0)
	mv $a1, $a0
	j factorial

factorial:
	subi $a1, $a1, 1
	beq $a1, $zero, 4
	mul $a0, $a0, $a1
	j factorial
	j end

end:
	mv $s0, $a0
	exit
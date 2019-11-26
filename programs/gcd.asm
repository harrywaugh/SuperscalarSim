.a 96
.b 57

main:
	la $t0, a
	lw $a0, $zero($t0)
	la $t0, b
	lw $a1, $zero($t0)
	j gcd

gcd:
	beq $a0, $zero, 10
	beq $a1, $zero, 7
	beq $a0, $a1, 6
	blt $a1, $a0, 3
	sub $a1, $a1, $a0
	j gcd
	sub $a0, $a0, $a1
	j gcd
	mv $s0, $a0
	j end
	mv $s0, $a1
	j end

end:
	exit

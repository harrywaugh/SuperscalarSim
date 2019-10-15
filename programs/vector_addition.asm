main:
    addi $s0, $zero, 1    # s0=0+1   s registers are "Saved values representing final computed results"
    addi $s1, $zero, 5    
    addi $a0, $zero, 0    # a registers represent the arguments to the program
    addi $a1, $zero, 1
    addi $a2, $zero, 2
    j for                 # jump to 'for' section

for:
    beq $s0, $s1, end     # branch if (s0 == s1), jump to 'end'
    lw $t1, 0($a1)        # load word from memory[a1+0] into temporary $t1
    lw $t2, 0($a2)
    add $t0, $t1, $t2     # t0 = t1 + t2
    sw $t0, 0($a0)
	addi $a0, $a0, 4
	addi $a1, $a1, 4
	addi $a2, $a2, 4
	addi $s0, $s0, 1
	j for

end:
	addi $v0, $s0, 0

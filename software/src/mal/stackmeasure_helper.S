   .set mips32r2
    .set nomips16
    .text
    .align 4

    .extern _splim, 4

    .global fill_stack
    .global stack_usage

    .equ    stack_fill_word, 0xAA5555AA

    .ent    fill_stack
fill_stack:
    la      $t0, _splim
    li      $t1, stack_fill_word
1:
    sltu    $t2, $sp, $t0
    bnez    $t2, 2f
    sw      $t1, 0($t0)
    addiu   $t0, 4
    b       1b
2:
    jr      $ra
    .end    fill_stack

    .ent    stack_usage
stack_usage:
    li      $v0, 0
    la      $t0, _splim
    li      $t1, stack_fill_word
1:
    sltu    $t2, $sp, $t0
    bnez    $t2, 2f
    lw      $t2, 0($t0)
    bne     $t2, $t1, 2f
    addiu   $v0, 1
    addiu   $t0, 4
    b       1b
2:
    jr      $ra
    .end    stack_usage

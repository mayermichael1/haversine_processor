global bandwidth_test

bandwidth_test:
	align 64
    mov r8, rsi
.loop:
    vmovdqu ymm0, [r8]
    vmovdqu ymm1, [r8 + 32]
    vmovdqu ymm2, [r8 + 64]
    vmovdqu ymm3, [r8 + 96]
    add rsi, 128
    sub rdi, 128 
    ja .loop
    ret


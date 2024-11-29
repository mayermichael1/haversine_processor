global bandwidth_test

bandwidth_test:
	align 64
    xor r8, r8
.loop:
    mov r9, rsi
    add r9, r8
    vmovdqu ymm0, [r9]
    vmovdqu ymm1, [r9 + 32]
    vmovdqu ymm2, [r9 + 64]
    vmovdqu ymm3, [r9 + 96]
    vmovdqu ymm0, [r9 + 128]
    vmovdqu ymm1, [r9 + 160]
    vmovdqu ymm2, [r9 + 192]
    vmovdqu ymm3, [r9 + 224]
    add r8, 256
    and r8, rdx
    sub rdi, 256 
    ja .loop
    ret


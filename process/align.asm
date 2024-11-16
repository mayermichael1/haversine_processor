global loop_aligned_64
global loop_aligned_64_offset_1
global loop_aligned_64_offset_5
global loop_aligned_64_offset_63

section .text

loop_aligned_64:
    xor rax, rax
align 64
.loop:
    inc rax
    cmp rax, rdi
    jb .loop
    ret

loop_aligned_64_offset_1:
    xor rax, rax
align 64
nop
.loop:
    inc rax
    cmp rax, rdi
    jb .loop
    ret

loop_aligned_64_offset_5:
    xor rax, rax
align 64
%rep 5
nop
%endrep
.loop:
    inc rax
    cmp rax, rdi
    jb .loop
    ret

loop_aligned_64_offset_63:
    xor rax, rax
align 64
%rep 63
nop
%endrep
.loop:
    inc rax
    cmp rax, rdi
    jb .loop
    ret


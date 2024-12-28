global nt_test 
global test 

; rdi, rsi, rdx, rcx

; rdi ... buffer size
; rdx ... data size
; rsi ... buffer
; rcx ... data buffer
test:
    align 64
    xor r10, r10    ; buffer offset
.buffer:
    xor r8, r8  ; data table offset

    .data:
        mov r9, rsi ; write offset
        add r9, r10

        vmovdqu xmm0, [rcx + r8]
        vmovdqu [r9], xmm0 

        add r8, 16
        add r10, 16
        cmp r8, rdx
        jb .data

    cmp r10, rdi
    jb .buffer 
    ret

nt_test:
    align 64
    xor r10, r10    ; buffer offset
.buffer:
    xor r8, r8  ; data table offset

    .data:
        mov r9, rsi ; write offset
        add r9, r10

        vmovdqu xmm0, [rcx + r8]
        vmovntdq [r9], xmm0 

        add r8, 16
        add r10, 16
        cmp r8, rdx
        jb .data

    cmp r10, rdi
    jb .buffer 
    ret


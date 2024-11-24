global read_x_1
global read_x_2
global read_x_3
global read_x_4
global read_x_5
global read_x_6

; rax ... return
; parameters ... rdi, rsi, rdx, rcs, r8, r9

read_x_1:
    align 64
.loop:
    mov rax, [rsi]
    sub rdi, 1
    jnle .loop
    ret

read_x_2:
    align 64
.loop:
    mov rax, [rsi]
    mov rax, [rsi]
    sub rdi, 2
    jnle .loop
    ret

read_x_3:
    align 64
.loop:
    mov rax, [rsi]
    mov rax, [rsi]
    mov rax, [rsi]
    sub rdi, 3
    jnle .loop
    ret
    
read_x_4:
    align 64
.loop:
    mov rax, [rsi]
    mov rax, [rsi]
    mov rax, [rsi]
    mov rax, [rsi]
    sub rdi, 4
    jnle .loop
    ret

read_x_5:
    align 64
.loop:
    mov rax, [rsi]
    mov rax, [rsi]
    mov rax, [rsi]
    mov rax, [rsi]
    mov rax, [rsi]
    sub rdi, 5
    jnle .loop
    ret

read_x_6:
    align 64
.loop:
    mov rax, [rsi]
    mov rax, [rsi]
    mov rax, [rsi]
    mov rax, [rsi]
    mov rax, [rsi]
    mov rax, [rsi]
    sub rdi, 6
    jnle .loop
    ret

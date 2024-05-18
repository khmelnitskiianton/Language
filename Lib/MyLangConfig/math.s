;#=======================#
;#     MATH FUNCTIONS    #
;#=======================#

INACCURACY  equ 2

global sqrt
global pow
extern error_end

;========================================================================================
section .data

TEN dq 10.0

section .text
;========================================================================================
;sqrt(a) - make pseudo sqrt from pseudo double
;Args: in stack pseudo double last number
;Returns: rax - pseudo double sqrt(a)
sqrt:
    mov rax, qword [rsp+8]    
    cmp rax, 0
    jg .check_negative
    mov rdi, 2
    call error_end
.check_negative:

    fild qword [rsp+8];convert int to FPU and push in FPU Stack ST(0):=num
    lea rax, TEN
    mov rcx, INACCURACY ;make from pseudo number - normal double
.divizion:
    fdiv qword [rax]
    loop .divizion
    
    fsqrt;take root, ST(0):=sqrt(num)

    mov rcx, INACCURACY ;make from pseudo number - normal double
.multiply:
    fmul qword [rax]
    loop .multiply

    fistp qword [rsp+8] ;in stack new value

    mov rax, qword [rsp+8] ;change value
    ret
;========================================================================================
;pow(a, b) - a power b
;Args: 2 in stack
;Return: rax a power b
pow:
    mov rax, qword [rsp + 16]
    mov rdx,0

    cmp rax, 0      ;check for < 0
    jge .end_check0
    mov rax, qword [rsp+8]
    ret
.end_check0:

    mov rbx, 10
    mov rcx, INACCURACY
.loop_1:
    idiv rbx
    loop .loop_1
    mov rbx, rax ; counter in rbx
    xor rax, rax

    cmp rbx, 0      ;check for 0
    jne .end_check1
    mov rax, 1
    mov rbx, 10
    mov rcx, INACCURACY
.loop_2:
    imul rbx
    loop .loop_2
    ret
.end_check1:   

    mov rax, qword [rsp+8]

    cmp rbx, 1      ;check for 1
    jne .end_check2
    ret
.end_check2:   

    mov r8, rax
.loop_3:
    imul r8

    jno .no_overflow2
    mov rdi, 0
    call error_end
.no_overflow2:

    push rbx
    mov rcx, INACCURACY
    
.loop_new2:
    cqo
    mov rbx, 10
    idiv rbx
    loop .loop_new2
    pop rbx

    dec rbx
    cmp rbx, 1
    ja .loop_3

    ret
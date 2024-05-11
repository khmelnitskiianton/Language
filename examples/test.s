;#=========================#
;#      Start module       #
;#=========================#

_stack_offset equ 8

global main

;#=========================#
;#      Main module        #
;#=========================#


;#=========Function========#
main:
;#=======Input=Action======#
push rbp
mov  rbp, rsp
sub  rsp, 32
;#=======End=Action========#

;#========Init=Local=======#
mov qword [rbp - _stack_offset*0], 0
mov qword [rbp - _stack_offset*1], 0
mov qword [rbp - _stack_offset*2], 0
mov qword [rbp - _stack_offset*3], 0
;#========End=Init=========#


;#========Var=Assign=======#

push 1

;assign
pop rax
mov qword [rbp - _stack_offset*0], rax
;#=========End=Var=========#

;#========Var=Assign=======#

push 6

;assign
pop rax
mov qword [rbp - _stack_offset*1], rax
;#=========End=Var=========#

;#========Var=Assign=======#

push qword [rbp - _stack_offset*0]

push qword [rbp - _stack_offset*1]

pop rbx      ; add
pop rax
add rax, rbx
push rax

push 20

push qword [rbp - _stack_offset*0]

pop rbx      ; imul
pop rax
mov rdx, 0
imul rbx
push rax

pop rbx      ; sub
pop rax
sub rax, rbx
push rax

;assign
pop rax
mov qword [rbp - _stack_offset*2], rax
;#=========End=Var=========#

jmp .while_check_0
.while_start_0:


;#========Var=Assign=======#

push 0

;assign
pop rax
mov qword [rbp - _stack_offset*3], rax
;#=========End=Var=========#

.while_check_0:

push qword [rbp - _stack_offset*0]
pop rax
cmp rax, 1
ja .while_start_0
.while_end___0:


;#========Var=Return=======#
mov rax, 0
mov rsp, rbp
pop  rbp
ret

;#=======Leave=Action======#
mov  rsp, rbp
pop  rbp
ret
;#=======End=Function======#

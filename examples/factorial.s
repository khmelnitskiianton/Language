;#=========================#
;# THIS FILE WAS GENERATED #
;#    BY ANTON'S LANGUAGE  #
;#GITHUB: khmelnitskiianton#
;#=========================#

;#=========================#
;#      Start module       #
;#=========================#
_stack_offset equ 8

global main

extern print
extern input
extern error_end
extern __processing_unassigned_var__

extern sqrt
extern pow

;#=========================#
;#      Main module        #
;#=========================#


;#=========Function========#
main:
;#=======Input=Action======#
push rbp
mov  rbp, rsp
sub  rsp, 8
;#=======End=Action========#

;#========Init=Local=======#
mov qword [rbp - _stack_offset*0 - 8], 0
mov qword [rbp - _stack_offset*1 - 8], 0
;#========End=Init=========#


;#========Var=Assign=======#
;#==========Call===========#

call input
add rsp,0
;#=========End=Call========#
push rax

;assign
pop rax
mov qword [rbp - _stack_offset*0 - 8], rax
;#=========End=Var=========#

;#========Var=Assign=======#
;#==========Call===========#

push qword [rbp - _stack_offset*0 - 8]

call factorial
add rsp,8
;#=========End=Call========#
push rax

;assign
pop rax
mov qword [rbp - _stack_offset*0 - 8], rax
;#=========End=Var=========#
;#==========Call===========#

push qword [rbp - _stack_offset*0 - 8]

call print
add rsp,8
;#=========End=Call========#

;#========Var=Return=======#

push 0

pop rax
mov rsp, rbp
pop  rbp
ret
;#=======End=Return======#

;#=======Leave=Action======#
mov  rsp, rbp
pop  rbp
ret
;#=======End=Function======#

;#=========Function========#
factorial:
;#=======Input=Action======#
push rbp
mov  rbp, rsp
sub  rsp, 0
;#=======End=Action========#

;#========Init=Local=======#
;#========End=Init=========#

;#=============If==========#
jmp .if_check_0
.if_start_0:

;#========Var=Return=======#

push 100

pop rax
mov rsp, rbp
pop  rbp
ret
;#=======End=Return======#
jmp .if_end_0
.if_check_0:
;#========Condition========#

push qword [rbp + _stack_offset*0 + 16]

push 100

pop rbx      ; below
pop rax
cmp rax, rbx
setl  al       ; set func 1 or 0
movzx rax, al  ; expand al to rax
mov rbx, 100
mul rbx
push rax

pop rax
cmp rax, 0
jne .if_start_0
.if_end_0:
;#=========End=IF=========#

;#========Var=Return=======#

push qword [rbp + _stack_offset*0 + 16]
;#==========Call===========#

push qword [rbp + _stack_offset*0 + 16]

push 100

pop rbx      ; sub
pop rax
sub rax, rbx
push rax

call factorial
add rsp,8
;#=========End=Call========#
push rax

pop rbx      ; imul
pop rax
cqo
imul rbx
jno .no_overflow_0
mov rdi, 0
call error_end
.no_overflow_0:
cqo
mov rbx, 100
idiv rbx
push rax

pop rax
mov rsp, rbp
pop  rbp
ret
;#=======End=Return======#

;#=======Leave=Action======#
mov  rsp, rbp
pop  rbp
ret
;#=======End=Function======#

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
sub  rsp, 16
;#=======End=Action========#

;#========Init=Local=======#
mov qword [rbp - _stack_offset*0], 0
mov qword [rbp - _stack_offset*1], 0
;#========End=Init=========#


jmp if_check_0
if_start_0:
;#==========Call===========#
push qword [rbp - _stack_offset*1]
push qword [rbp - _stack_offset*0]
push 0

call sum
;#=========End=Call========#
jmp if_end___0
if_check_0:
mov rax, qword [rbp - _stack_offset*0]
mov rbx, 0
cmp rax, rbx
je if_start_0
if_end___0:


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

;#=========Function========#
sum:
;#=======Input=Action======#
push rbp
mov  rbp, rsp
sub  rsp, 16
;#=======End=Action========#

;#========Init=Local=======#
mov qword [rbp - _stack_offset*0], 0
mov qword [rbp - _stack_offset*1], 0
;#========End=Init=========#


;#=======Void=Return=======#
mov    rsp, rbp
pop    rbp
ret
;#=========End=Return========#


;#=======Leave=Action======#
mov  rsp, rbp
pop  rbp
ret
;#=======End=Function======#

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
extern __processing_undefined_var__

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
;#========End=Init=========#


;#========Var=Assign=======#
;#==========Call===========#
push 2
push 1

call summ
add rsp,16
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
summ:
;#=======Input=Action======#
push rbp
mov  rbp, rsp
sub  rsp, 8
;#=======End=Action========#

;#========Init=Local=======#
mov qword [rbp - _stack_offset*0 - 8], 0
;#========End=Init=========#


;#========Var=Assign=======#

push qword [rbp + _stack_offset*0 + 16]

push qword [rbp + _stack_offset*1 + 16]

pop rbx      ; add
pop rax
add rax, rbx
push rax

;assign
pop rax
mov qword [rbp - _stack_offset*0 - 8], rax
;#=========End=Var=========#

;#========Var=Return=======#
mov rax, qword [rbp - _stack_offset*0 - 8]
mov rsp, rbp
pop  rbp
ret

;#=======Leave=Action======#
mov  rsp, rbp
pop  rbp
ret
;#=======End=Function======#

;#============#
;#Start module#
;#============#

_stack_offset equ 8

global main

;#===========#
;#Main module#
;#===========#

main:
push rbp
mov  rbp, rsp
sub  rsp, 8

mov qword [rbp - _stack_offset*0], 0

mov rax, qword [rbp - _stack_offset*0]
mov rsp, rbp
pop  rbp
ret

mov rsp, rbp
pop  rbp
ret


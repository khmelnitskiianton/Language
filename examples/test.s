;#============#
;#Start module#
;#============#

global _start

section .text

_stack_offset equ 8

_start:
	call main
	mov rax, 60
	mov rdi, 0
	syscall

;#===========#
;#Main module#
;#===========#

main:
push rbp
mov  rbp, rsp
sub  rsp, 16

mov qword [rbp - _stack_offset*0], 0
mov qword [rbp - _stack_offset*1], 0

pop  rbp
ret

summ:
push rbp
mov  rbp, rsp
sub  rsp, 0


mov rax, qword [rbp + _stack_offset*0]
pop  rbp
ret

pop  rbp
ret


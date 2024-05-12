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
mov qword [rbp - _stack_offset*0], 0
;#========End=Init=========#


;#========Var=Assign=======#
;#==========Call===========#

call input
add rsp,0
;#=========End=Call========#
push rax

;assign
pop rax
mov qword [rbp - _stack_offset*0], rax
;#=========End=Var=========#
;#==========Call===========#
push qword [rbp - _stack_offset*0]

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

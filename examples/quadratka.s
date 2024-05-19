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
extern puts
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
sub  rsp, 24
;#=======End=Action========#

;#========Init=Local=======#
mov qword [rbp - _stack_offset*0 - 8], 0
mov qword [rbp - _stack_offset*1 - 8], 0
mov qword [rbp - _stack_offset*2 - 8], 0
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

call input
add rsp,0
;#=========End=Call========#
push rax

;assign
pop rax
mov qword [rbp - _stack_offset*1 - 8], rax
;#=========End=Var=========#

;#========Var=Assign=======#
;#==========Call===========#

call input
add rsp,0
;#=========End=Call========#
push rax

;assign
pop rax
mov qword [rbp - _stack_offset*2 - 8], rax
;#=========End=Var=========#
;#=============If==========#
jmp .if_check_0
.if_start_0:
;#=============If==========#
jmp .if_check_1
.if_start_1:
;#=============If==========#
jmp .if_check_2
.if_start_2:
;#==========Call===========#

push 0

push 1300

push 1000

push 7000

push 7800

push 7300

call puts
add rsp,48
;#=========End=Call========#
jmp .if_end_2
.if_check_2:
;#========Condition========#

push qword [rbp - _stack_offset*2 - 8]

push 0

pop rbx      ; equal
pop rax
cmp rax, rbx
sete  al
     ; set func 1 or 0
movzx rax, al  ; expand al to rax
mov rbx, 100
mul rbx
push rax

pop rax
cmp rax, 0
jne .if_start_2
;#==========Call===========#

push 0

push 3200

push 6100

push 3200

push 12000

call puts
add rsp,40
;#=========End=Call========#
;#==========Call===========#

push 0

call print
add rsp,8
;#=========End=Call========#
;#==========Call===========#

push 0

push 1000

call puts
add rsp,16
;#=========End=Call========#
.if_end_2:

;#=========End=IF=========#
jmp .if_end_1
.if_check_1:
;#========Condition========#

push qword [rbp - _stack_offset*1 - 8]

push 0

pop rbx      ; equal
pop rax
cmp rax, rbx
sete  al
     ; set func 1 or 0
movzx rax, al  ; expand al to rax
mov rbx, 100
mul rbx
push rax

pop rax
cmp rax, 0
jne .if_start_1
.if_end_1:
;#=========End=IF=========#
jmp .if_end_0
.if_check_0:
;#========Condition========#

push qword [rbp - _stack_offset*0 - 8]

push 0

pop rbx      ; equal
pop rax
cmp rax, rbx
sete  al
     ; set func 1 or 0
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

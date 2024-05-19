;#=======================#
;#  STANDART FUNCTIONS   #
;#=======================#

global  input
global  print
global  puts
global  error_end
global  __processing_unassigned_var__

INACCURACY      equ 2

SYSCALL_WRITE   equ 0x01
SYSCALL_READ    equ 0x0
SIZE_BUFFER     equ 50
_stack_offset   equ 8
DOT             equ '.'
ADDRESS_CALL    equ 16

;=======================================================================================
;Functions

section .text

;========================================================================================
;input(void) - function of input one number float/integer
;Arg: void
;Ret: RAX-number
input:
        nop
        ;input action
        push rbp        ; save rbp
        mov rbp, rsp    ; new rbp = rsp
        
        push rcx        ;save regs
        push rdx
        push rsi
        push rdi          
        push rbx 
        push r8
        push r9
        push r10

        call read_buff 
        
        mov rsi, buffer_in
        call string_to_pseudo_float ;in rax number

        push rax
        mov rdi, buffer_in     ;clear buffer 
        mov rax, 0
        mov rcx, SIZE_BUFFER
        rep stosb
        pop rax

        pop r10
        pop r9
        pop r8
        pop rbx
        pop rdi         ;revive regs
        pop rsi
        pop rdx
        pop rcx

        leave           ;mov rsp, rbp ; free stack from local vars and all rubbish
                        ;pop rbp      ; revive rbp        
        ret
;========================================================================================
;print(var x) - function of output one signed number 
;Arg - in stack one number
;Ret: rax = 0
;WARNING: dont save rdi!
print: 
        nop

        pop r10
        pop rdi         ;for call
        
        push r9         ; save all regs in stack
        push r8         ; now i have all args in stack
        push rcx
        push rdx
        push rsi
        
        push rbp        ;save regs by convention
        push rbx 
        push r12 
        push r13 
        push r14 
        push r15

        xor r11, r11            ;clean r11
        xor r15, r15

        call print_pseudo_float

        cmp r11, 0              ;last check buffer and print it
        je .if_buff_end 
        call write_buff
.if_buff_end:

        mov rdi, buffer_out     ;clear buffer 
        mov rax, 0
        mov rcx, SIZE_BUFFER
        rep stosb

        pop r15         ;revive regs
        pop r14
        pop r13
        pop r12
        pop rbx
        pop rbp

        pop rsi
        pop rdx
        pop rcx
        pop r8
        pop r9

        mov rax, 0

        push rdi
        push r10        ;put in stack address of return from Printf32

        ret
;========================================================================================
;puts(askii code, ..., 0) - function of write string to stdout
;Args: decimal askii codes of symbols, last - 0 - terminated symbol
;Ret: rax = 0
;Change: rdi, rbx, WARNING: R11- global counter
puts:
        nop
        push rbp
        mov  rbp, rsp

        push r11
        push r15
        push rdi        ;save regs
        push rdx
        push rbx
        push rcx

        xor r11, r11    ;init regs
        xor r15, r15

        ;while(*ch != '\0') {print_char(*ch)} "print_char" means move to buffer
        mov rbx, rbp     ; in rbx - address of first char
        add rbx, ADDRESS_CALL
.while_start:
        mov rax, [rbx]          ;pseudo double number cast to char
        
        cmp rax, 0    ; *ch != '\0'
        je .while_end
        
        mov rcx, INACCURACY
        mov rdi, 10
.dividing:
        cqo
        div rdi
        loop .dividing  ;in ral - char
        xor rdi, rdi         ;set rdi = 0
        mov dil, al  ;mov rdi(dil = 1 byte), [rbx] 

        call print_char ;call print current char

        ;set next symbol
        add rbx, _stack_offset
        jmp .while_start
.while_end:
        cmp r11, 0              ;last check buffer and print it
        je .if_buff_end 
        call write_buff
.if_buff_end:

        mov rdi, buffer_out     ;clear buffer 
        mov rax, 0
        mov rcx, SIZE_BUFFER
        rep stosb

        pop rcx
        pop rbx         ;revive regs
        pop rdx
        pop rdi
        pop r11
        
        mov rax, 0

        mov  rsp, rbp
        pop  rbp
        ret
;========================================================================================
;End program without segfault
;Args: RDI - code of error
error_end:
        xor r11, r11            ;clean r11
        xor r15, r15

        mov rbx, [rdi*8 + jmp_table_errors] ;count address of buffer 
        mov rdi, rbx
        call print_str
        cmp r11, 0              ;last check buffer and print it
        je .if_buff_end 
        call write_buff
.if_buff_end:
        ;write "\n"

        mov rax, 60
        syscall
;========================================================================================
;==================================HELP=FUNCTIONS========================================
;write_buff() - function of write buffer in stdout
;Args: ABI  "buffer_out" - buffer with string
;Ret: void
;Change: nothing
write_buff:
        push r11
        push rax        ;save regs of syscall
        push rcx
        push rsi
        push rdx
        push rdi

        add r15, r11    ;global 

        mov rax, SYSCALL_WRITE  ; syscall 0x01: write(rdi, rsi, rdx) - (int, char*, size_t)
        mov rdi, 1              ; stdout
        mov rsi, buffer_out     ; address of str - in stack last char
        mov rdx, SIZE_BUFFER    ; length
        syscall
                
        pop rdi          ;revive regs
        pop rdx        
        pop rsi
        pop rcx
        pop rax  
        pop r11

        ret 
;----------------------------------------------------------------------------------------
;read_buff() - function of write buffer in stdout
;Args: ABI , "buffer_in" - buffer to write string from output
;Ret: void
;Change: nothing
read_buff:
        push r11
        push rax        ;save regs of syscall
        push rcx
        push rsi
        push rdx
        push rdi

        mov rax, SYSCALL_READ   ; syscall 0x0: read(rdi, rsi, rdx) - (int, char*, size_t)
        mov rdi, 0              ; stdin file descriptor
        mov rsi, buffer_in      ; address of str - in stack last char
        mov rdx, SIZE_BUFFER    ; length
        syscall
                
        pop rdi          ;revive regs
        pop rdx        
        pop rsi
        pop rcx
        pop rax  
        pop r11

        ret 
;========================================================================================
; string_to_int(): convert str to pseudo float
; RSI = pointer to the string 
; Output:
; RAX = integer pseudo float number
string_to_pseudo_float:

        ;FORMULA:
        ;Normal: 3.14 -> 3.14*100 = 314
        ;In runtime: 3.14 = 3 + 0.14 -> 3*100 + 0.14*10!
        ; part before dot in r9, part after in rax!

        xor r8,r8        ; flag for have dot!
        xor r9,r9  
        xor r10, r10
        mov rcx, INACCURACY 
        xor rbx,rbx     ; clear ebx
        movzx rdx, byte[rsi]
        cmp rdx, '-'    ; check for number < 0
        mov rdx,rsi
        jne .end_case
        inc rdx
.end_case:
.next_digit:
        movzx rax, byte[rdx]
        cmp rax, DOT
        jne .continue_check
        mov r8, 1
        inc rdx
        movzx rax, byte[rdx]
        mov r9, rbx     ;in r9 part before dot
        xor rbx,rbx
.continue_check:
        cmp rax, '0'
        jb .end_convert
        cmp rax, '9'
        ja .end_convert       
.success_check:
        inc rdx
        sub al,'0'    ; convert from ASCII to number
        
        xor r10, r10

        cmp r8, 1          ;if (find dot)
        jne .calc_no_dot
        ;pow(rbx, 10) + rax
        mov r10, rcx
        push rbx
        push rdx
        jmp .check_cond
.imul_inaccurancy:      ;in r10 - times of mul 10
        mov rbx, 10
        imul rbx
        
        jno .check_overflow1
        mov rdi, 0
        call error_end
.check_overflow1:

.check_cond:
        cmp r10, 0
        dec r10
        ja .imul_inaccurancy
        pop rdx
        pop rbx
        add rbx, rax 
        jmp .end_calc
.calc_no_dot:
        imul rbx, 10


        jno .check_overflow0
        mov rdi, 0
        call error_end
.check_overflow0:

        add rbx,rax   ; ebx = ebx*10 + eax
.end_calc:

        cmp r8, 1          ;if (find dot)
        jne .end_dot_while ;if find dot, we process next some digits, not all
.dot_while:
        dec rcx
        jnz .next_digit
        jmp .end_convert
.end_dot_while:
        jmp .next_digit  
.end_convert:
        cmp r8, 1 
        jne .no_dot_found
        mov rax,rbx
        jmp .end_end
.no_dot_found:
        mov r9, rbx
        xor rbx,rbx
        xor rax,rax
.end_end:

        mov rcx, INACCURACY
.imul_inaccurancy_before:
        cqo
        mov rbx, 10     ; add pseudo accurancy
        imul r9, rbx

        jno .check_overflow2
        mov rdi, 0
        call error_end
.check_overflow2:

        loop .imul_inaccurancy_before

        add rax, r9

.check_minus:
        movzx rdx, byte[rsi]
        cmp rdx, '-'    ; check for number < 0
        jne .end_minus_check
        neg rax
.end_minus_check:
        ret
;========================================================================================
;print_char(int a) - function of write one char symbol in stdout
;Args: ABI - argument in rdi, r11 - global counter of buffer
;Ret: void
;Change: buffer_out, r11
print_char:
        nop
        ;if (size_buff<=r11) {write, clear}
.if_start:
        cmp r11, SIZE_BUFFER
        jb .if_end 
 
        call write_buff

        push rcx
        mov rcx, r11
.for_start:
        mov buffer_out[rcx], byte 0
        loop .for_start
        pop rcx
        mov r11, 0
.if_end:
        mov buffer_out[r11], dil
        inc r11
        ret

;========================================================================================        
__processing_unassigned_var__:
        mov rax, SYSCALL_WRITE  ; syscall 0x01: write(rdi, rsi, rdx) - (int, char*, size_t)
        mov rdi, 1              ; stdout
        mov rsi, msg     ; address of str - in stack last char
        mov rdx, msg_len    ; length
        syscall
        mov rbx, 0
        idiv rbx
        ret
;========================================================================================
;print_str(const char* buff) - function of write string in stdout
;Args: ABI - argument in rdi
;Ret: void
;Change: rdi, rbx
print_str:
        nop
        push rdi        ;save regs
        push rbx
        ;while(*ch != '\0') {print_char(*ch)}
        mov rbx, rdi   ;rbx - address on buffer
.while_start:
        cmp byte [rbx], 0    ; *ch != '\0'
        je .while_end
        xor rdi, rdi         ;set rdi = 0
        mov dil, byte [rbx]  ;mov rdi(dil = 1 byte), [rbx] 

        call print_char ;call print current char

        ;set next symbol
        inc rbx
        jmp .while_start
.while_end:
        pop rbx         ;revive regs
        pop rdi
        ret
;=========================================================================================
;print_pseudo_float(int a) - function of write pseudo float with fix inaccuracy(3.141 -> 314 -> 3  .  14) printing dot
;Args: ABI - rdi - number
;Ret: rax = 0
;Change: rsi, rcx, rdi, rax, rdx, r8, r9
print_pseudo_float:
        nop

        push rsi        ;save regs
        push rcx
        push rdi
        push rax
        push rdx
        push rbx
        push r8
        push r9
        push r10

        mov rbx, INACCURACY     ;rbx - number of digits before dot!
        xor r10,r10             ; r10 - flag of printing dot

        mov rax, rdi     ;rax = number
        mov r8, 10
        cmp rax, 0       ;check for negative
        jl .case_neg
        jmp .end_case
.case_neg:              ;case of negative number
        neg eax         ;Make signed 32byte number - unsigned
        mov dil, '-'
        call print_char ;call print current char
.end_case:
        ;printing digits
        ;div: rax - quotient, rdx - remains 
        xor rcx, rcx    ;rcx - counter of digits
.while_start:           ;do{...}while(rax != 0)
        cqo             ;expand rax to rdx:rax for dividing
        idiv r8  

        xor r9, r9
        mov r9b, dec_str[rdx]
        push r9
        inc rcx         ;inc rcx

.dot_check:             ; check for printing dot
        cmp rbx, rcx
        jne .end_dot_check
        push DOT
        mov r10, 1
        cmp rax, 0
        jne .end_dot_check
        push '0'
        inc rcx
.end_dot_check:

        cmp rax, 0
        je .while_end
        jmp .while_start
.while_end:
        xor rdi, rdi
        cmp r10, 1
        jne .no_dot 
        inc rcx         ;inc for place for dot in stack!
.no_dot:
.for_begin:             ;loop: print all digits from stack
        pop rdi         
        call print_char ;print char
        loop .for_begin

        pop r10
        pop r9
        pop r8
        pop rbx
        pop rdx         ;revive regs
        pop rax
        pop rdi
        pop rcx
        pop rsi
        mov rax, 0
        ret
;==================================================================
;NEVER USED
;num_length(int a)
;Args: rdi - number
;Return: rax - length of digits
num_length:

        push rdi
        push r8
        push rcx
        push r9

        mov rax, rdi    ;save number in rbx
        mov r8, 10
        cmp eax, 0      ;check for -
        jl .case_neg
        jmp .end_case
.case_neg:              ;case of negative number
        neg eax         ;Make signed 32byte number - unsigned
.end_case:
        xor rcx, rcx    ;counter of digits
.while_start:           ;do{...}while(rbx != 0)
        cqo             ;expand rax to rdx:rax for dividing
        div r8       
        xor r9, r9
        mov r9b, dec_str[rdx]
        inc rcx         ;inc rcx
        cmp rax, 0
        je .while_end
        jmp .while_start
.while_end:
        mov rax, rcx

        pop r9
        pop rcx
        pop r8
        pop rdi

        ret

;========================================================================================
section .rodata
dec_str         db "0123456789", '\0' ;const alphabets for number systems
msg             db "\r\n\r\n> Нельзя просто так взять и обратится к неинициализированной переменной. Боромир\r\n\r\n> One does not simply refer to unassigned variable. Boromir\r\n\r\n", 0
msg_len         equ $ - msg

;#Errors
error_list1 db "> Overflowing in pseudo double number! Fuck its so big!\r\n\r\n", 0
error_list2 db "> Divizion by zero! Bruh man...\r\n\r\n",'\0'
error_list3 db "> Shit happens! Number is negative in sqrt()!\r\n\r\n",0

;#Jump table of errors!
jmp_table_errors:
        dq error_list1
        dq error_list2
        dq error_list3

;=======================================================
;Buffers
section .data
buffer_out:     times SIZE_BUFFER db 0 ;buffer for write
buffer_in:      times SIZE_BUFFER db 0 ;buffer for input
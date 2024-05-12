;#=======================#
;#  STANDART FUNCTIONS   #
;#=======================#

SYSCALL_WRITE   equ 0x01
SYSCALL_READ    equ 0x0
SIZE_BUFFER     equ 50
_stack_offset   equ 8
SUCCESS_RET     equ 1
FAIL_RET        equ 0

global input
global print
global __SeG_FaUlT__

section .text

;========================================================================================
;input(void) - function of input one siged number
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

        call read_buff 
        
        mov rsi, buffer_in
        call string_to_int ;in rax number

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
;Ret: number in rax
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

        call print_dec_sign

        cmp r11, 0              ;last check buffer and print it
        je .if_buff_end 
        call write_buff
.if_buff_end:
        ;write "\n"
        mov rdi, buffer_out     ;clear buffer 
        mov rax, 0
        mov rcx, SIZE_BUFFER
        rep stosb
        mov byte buffer_out[0], 0xA
        mov byte buffer_out[1], 0xD
        call write_buff

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

        push rdi

        push r10        ;put in stack address of return from Printf32

        mov rax, 0

        ret

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
; string_to_int():
; RSI = pointer to the string to convert
; Output:
; RAX = integer value remember its 32 byte in eax
string_to_int:
        xor rbx,rbx     ; clear ebx
        movzx rdx, byte[rsi]
        cmp rdx, '-'    ; check for number < 0
        mov rdx,rsi
        jne .end_case
        inc rdx
.end_case:
.next_digit:
        movzx rax, byte[rdx]
        cmp rax, '0'
        jb .end_convert
        cmp rax, '9'
        ja .end_convert       
.success_check:
        inc rdx
        sub al,'0'    ; convert from ASCII to number
        imul rbx,10
        add rbx,rax   ; ebx = ebx*10 + eax
        jmp .next_digit  ; while (--rcx)
.end_convert:
        mov rax,rbx
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
;=========================================================================================
;print_dec_sign(int a) - function of write decimal number
;Args: ABI - argument in rdi
;Ret: void
;Change: rsi, rcx, rdi, rax, rdx, r8, r9
print_dec_sign:
        nop

        push rsi        ;save regs
        push rcx
        push rdi
        push rax
        push rdx

        mov rax, rdi     ;rax = number
        mov r8, 10
        cmp eax, 0       ;Use eax, because its 32byte signed number
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
        xor rdx, rdx
.while_start:           ;do{...}while(rax != 0)
        cqo             ;expand rax to rdx:rax for dividing
        div r8       
        xor r9, r9
        mov r9b, dec_str[rdx]
        push r9
        inc rcx         ;inc rcx
        cmp rax, 0
        je .while_end
        jmp .while_start
.while_end:
        xor rdi, rdi
.for_begin:             ;loop: print all digits from stack
        pop rdi         
        call print_char ;print char
        loop .for_begin

        pop rdx         ;revive regs
        pop rax
        pop rdi
        pop rcx
        pop rsi

        ret
;#========================================================================================
section .rodata
dec_str         db "0123456789" ;const alphabets for number systems

section .data
buffer_out:     times SIZE_BUFFER db 0 ;buffer for write
buffer_in:      times SIZE_BUFFER db 0 ;buffer for input
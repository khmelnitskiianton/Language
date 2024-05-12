#include <stdio.h>
#include <stdlib.h>

#include "tree.h"
#include "MyLangConfig.h"

//left in rdx:rax, right in rbx, result in stack

int     _ADD       (FILE* FileProc, int label_counter){
    fprintf(FileProc,   "\npop rbx      ; add\n"
                        "pop rax\n"
                        "add rax, rbx\n"
                        "push rax\n");
    return 0;
}
int     _SUB       (FILE* FileProc, int label_counter){
    fprintf(FileProc,   "\npop rbx      ; sub\n"
                        "pop rax\n"
                        "sub rax, rbx\n"
                        "push rax\n");
    return 0;
}
int     _UADD      (FILE* FileProc, int label_counter){
    fprintf(FileProc,   "\npop rax      ; inc\n"
                        "inc rax\n"
                        "push rax\n");
    return 0;
}
int     _USUB      (FILE* FileProc, int label_counter){
    fprintf(FileProc,   "\npop rax      ; dec\n"
                        "dec rax\n"
                        "push rax\n");
    return 0;
}
int     _MUL       (FILE* FileProc, int label_counter){
    fprintf(FileProc,   "\npop rbx      ; imul\n"
                        "pop rax\n"
                        "mov rdx, 0\n"
                        "imul rbx\n"
                        "push rax\n"); //TODO: call func in asm ERROR, Check CF 
    return 0;
}
int     _DIV       (FILE* FileProc, int label_counter){
    fprintf(FileProc,   "\npop rbx      ; idiv\n"
                        "pop rax\n"
                        "mov rdx, 0\n"
                        "cqo            ; expand bit of sign in rdx from rax\n"
                        "cmp rbx, 0     ; check for zero\n"
                        "je error_end\n" 
                        "idiv rbx\n"
                        "push rax\n"); //TODO: call func in asm ERROR, Check CF 
    return 0;
}
int     _MOD       (FILE* FileProc, int label_counter){
    fprintf(FileProc,   "\npop rbx      ; idiv\n"
                        "pop rax\n"
                        "mov rdx, 0\n"
                        "cqo            ; expand bit of sign in rdx from rax\n"
                        "cmp rbx, 0     ; check for zero\n"
                        "je error_end\n" 
                        "idiv rbx\n"
                        "push rdx\n"); //TODO: call func in asm ERROR, Check CF 
    return 0;
}

//=======================================================================================

int     _NOT       (FILE* FileProc, int label_counter){
    fprintf(FileProc,   "\npop rax      ; not\n"
                        "cmp rax, 0\n"
                        "sete al        ; set func 1 or 0\n"
                        "movzx rax, al  ; expand al to rax\n"
                        "push rax\n");
    return 0;
}
int     _OR        (FILE* FileProc, int label_counter){
    fprintf(FileProc,   "\npop rbx      ; or\n"
                        "pop rax\n"
                        "cmp rax, 0\n"
                        "jne .L%d_1\n"
                        "cmp rbx, 0\n"
                        "je  .L%d_2\n"
                        ".L%d_1:\n"
                        "mov rax, 1\n"
                        "jmp .L%d_3\n"
                        ".L%d_2:\n"
                        "mov rax, 0\n"
                        ".L%d_3:\n"
                        "movzx rax, al  ; expand al to rax\n"
                        "push rax\n", label_counter,label_counter,label_counter,label_counter,label_counter,label_counter);    
    return 1;
}
int     _AND       (FILE* FileProc, int label_counter){
    fprintf(FileProc,   "\npop rbx      ; or\n"
                        "pop rax\n"
                        "cmp rax, 0\n"
                        "je  .L%d_1\n"
                        "cmp rbx, 0\n"
                        "je  .L%d_1\n"
                        "mov rax, 1\n"
                        "jmp .L%d_2\n"
                        ".L%d_1:\n"
                        "mov rax, 0\n"
                        ".L%d_2:\n"
                        "movzx rax, al  ; expand al to rax\n"
                        "push rax\n", label_counter,label_counter,label_counter,label_counter,label_counter);   
    return 1;
}

//=======================================================================================

int     _ABO       (FILE* FileProc, int label_counter){
    fprintf(FileProc,   "\npop rbx      ; above\n"
                        "pop rax\n"
                        "cmp rax, rbx\n"
                        "setg  al       ; set func 1 or 0\n"
                        "movzx rax, al  ; expand al to rax\n"
                        "push rax\n");
    return 0;
}
int     _BEL       (FILE* FileProc, int label_counter){
    fprintf(FileProc,   "\npop rbx      ; below\n"
                        "pop rax\n"
                        "cmp rax, rbx\n"
                        "setl  al       ; set func 1 or 0\n"
                        "movzx rax, al  ; expand al to rax\n"
                        "push rax\n");
    return 0;
}
int     _ABOEQU    (FILE* FileProc, int label_counter){
    fprintf(FileProc,   "\npop rbx      ; above or equal\n"
                        "pop rax\n"
                        "cmp rax, rbx\n"
                        "setge al      ; set func 1 or 0\n"
                        "movzx rax, al  ; expand al to rax\n"
                        "push rax\n");
    return 0;
}
int     _BELEQU    (FILE* FileProc, int label_counter){
    fprintf(FileProc,   "\npop rbx      ; below or equal\n"
                        "pop rax\n"
                        "cmp rax, rbx\n"
                        "setle al      ; set func 1 or 0\n"
                        "movzx rax, al  ; expand al to rax\n"
                        "push rax\n");
    return 0;
}
int     _NEQU      (FILE* FileProc, int label_counter){
    fprintf(FileProc,   "\npop rbx      ; not equal\n"
                        "pop rax\n"
                        "cmp rax, rbx\n"
                        "setne al\n     ; set func 1 or 0\n"
                        "movzx rax, al  ; expand al to rax\n"
                        "push rax\n");
    return 0;
}
int     _EQU       (FILE* FileProc, int label_counter){
    fprintf(FileProc,   "\npop rbx      ; equal\n"
                        "pop rax\n"
                        "cmp rax, rbx\n"
                        "sete  al\n     ; set func 1 or 0\n"
                        "movzx rax, al  ; expand al to rax\n"
                        "push rax\n");
    return 0;
}
#include <stdio.h>
#include <stdlib.h>

#include "tree.h"
#include "dsl.h"

#include "MyLangConfig.h"

//_ADD & _SUB in translator.cpp because they need access to arrays 

//left in rdx:rax, right in rbx, result in stack

int _ADD(BinaryTree_t* myTree, FILE* FileProc, Node_t* CurrentNode , int label_counter, void (*RecEvaluate) (BinaryTree_t* , Node_t*)){
    UNUSED(CurrentNode);
    UNUSED(label_counter);
    if ((R) && (!L)) 
    {
        RecEvaluate(myTree, R);
        return 0;
    }
    fprintf(FileProc,   "\npop rbx      ; add\n"
                        "pop rax\n"
                        "add rax, rbx\n"
                        "push rax\n");
    return 0;
}
int _SUB(BinaryTree_t* myTree, FILE* FileProc, Node_t* CurrentNode , int label_counter, void (*RecEvaluate) (BinaryTree_t* , Node_t*)){
    UNUSED(CurrentNode);
    UNUSED(label_counter);
    if ((R) && (!L)) //case: var x = -(x+y);
    {
        RecEvaluate(myTree, R);
        fprintf(FileProc,   "\npop rax      ; unary minus\n"
                            "mov rbx, -1\n"
                            "imul rbx\n"
                            "push rax\n");
        return 0;
    }
    fprintf(FileProc,   "\npop rbx      ; sub\n"
                        "pop rax\n"
                        "sub rax, rbx\n"
                        "push rax\n");
    return 0;
}

int _USUB(BinaryTree_t* myTree, FILE* FileProc, Node_t* CurrentNode , int label_counter, void (*RecEvaluate) (BinaryTree_t* , Node_t*)){
    UNUSED(myTree);
    UNUSED(FileProc);
    UNUSED(CurrentNode);
    UNUSED(label_counter);
    UNUSED(RecEvaluate);
    return 0;
}
int _UADD(BinaryTree_t* myTree, FILE* FileProc, Node_t* CurrentNode , int label_counter, void (*RecEvaluate) (BinaryTree_t* , Node_t*)){
    UNUSED(myTree);
    UNUSED(FileProc);
    UNUSED(CurrentNode);
    UNUSED(label_counter);
    UNUSED(RecEvaluate);
    return 0;
}
int _MUL(BinaryTree_t* myTree, FILE* FileProc, Node_t* CurrentNode , int label_counter, void (*RecEvaluate) (BinaryTree_t* , Node_t*)){
    UNUSED(myTree);
    UNUSED(CurrentNode);
    UNUSED(RecEvaluate);
    fprintf(FileProc,   "\npop rbx      ; imul\n"
                        "pop rax\n"
                        "cqo\n"
                        "imul rbx\n"
                        
                        "jno .no_overflow_%d\n"
                        "mov rdi, 0\n"
                        "call error_end\n"
                        ".no_overflow_%d:\n"
                        
                        "cqo\n"
                        "mov rbx, %d\n"
                        "idiv rbx\n"

                        "push rax\n", label_counter, label_counter, INACCURACY);
    return 1;
}
int _DIV(BinaryTree_t* myTree, FILE* FileProc, Node_t* CurrentNode , int label_counter, void (*RecEvaluate) (BinaryTree_t* , Node_t*)){
    UNUSED(myTree);
    UNUSED(CurrentNode);
    UNUSED(RecEvaluate);
    fprintf(FileProc,   "\npop rbx      ; idiv\n"
                        "pop rax\n"
                        "cqo            ; expand bit of sign in rdx from rax\n"
                        
                        "cmp rbx, 0     ; check for zero\n"
                        "jne .no_div_zero_%d\n" 
                        "mov rdi, 1\n"
                        "call error_end\n"
                        ".no_div_zero_%d:\n"
                        
                        "push rbx\n"
                        "cqo\n"
                        "mov rbx, %d\n"
                        "imul rbx\n"
                        "pop rbx\n"

                        "cqo\n"
                        "idiv rbx\n"

                        "push rax\n", label_counter, label_counter, INACCURACY); 
    return 1;
}
int _MOD(BinaryTree_t* myTree, FILE* FileProc, Node_t* CurrentNode , int label_counter, void (*RecEvaluate) (BinaryTree_t* , Node_t*)){
    UNUSED(myTree);
    UNUSED(CurrentNode);
    UNUSED(RecEvaluate);
    fprintf(FileProc,   "\npop rbx      ; idiv\n"
                        "pop rax\n"
                        "cqo            ; expand bit of sign in rdx from rax\n"
                        
                        "cmp rbx, 0     ; check for zero\n"
                        "jne .no_div_zero_%d\n" 
                        "mov rdi, 1\n"
                        "call error_end\n"
                        ".no_div_zero_%d:\n"
                        
                        "push rbx\n"
                        "cqo\n"
                        "mov rbx, %d\n"
                        "imul rbx\n"
                        "pop rbx\n"

                        "cqo\n"
                        "idiv rbx\n"

                        "push rdx\n", label_counter, label_counter, INACCURACY);
    return 1;
}

//=======================================================================================

int _NOT(BinaryTree_t* myTree, FILE* FileProc, Node_t* CurrentNode , int label_counter, void (*RecEvaluate) (BinaryTree_t* , Node_t*)){
    UNUSED(myTree);
    UNUSED(CurrentNode);
    UNUSED(label_counter);
    UNUSED(RecEvaluate);
    fprintf(FileProc,   "\npop rax      ; not\n"
                        "cmp rax, 0\n"
                        "sete al        ; set func 1 or 0\n"
                        "movzx rax, al  ; expand al to rax\n"
                        "mov rbx, %d\n"
                        "mul rbx\n"
                        "push rax\n", INACCURACY);
    return 0;
}
int _OR(BinaryTree_t* myTree, FILE* FileProc, Node_t* CurrentNode , int label_counter, void (*RecEvaluate) (BinaryTree_t* , Node_t*)){
    UNUSED(myTree);
    UNUSED(CurrentNode);
    UNUSED(RecEvaluate);
    fprintf(FileProc,   "\npop rbx      ; or\n"
                        "pop rax\n"
                        "cmp rax, 0\n"
                        "jne .L%d_1\n"
                        "cmp rbx, 0\n"
                        "je  .L%d_2\n"
                        ".L%d_1:\n"
                        "mov rax, %d\n"
                        "jmp .L%d_3\n"
                        ".L%d_2:\n"
                        "mov rax, 0\n"
                        ".L%d_3:\n"
                        "movzx rax, al  ; expand al to rax\n"
                        "push rax\n", label_counter,label_counter,label_counter, INACCURACY,label_counter,label_counter,label_counter);    
    return 1;
}
int _AND(BinaryTree_t* myTree, FILE* FileProc, Node_t* CurrentNode , int label_counter, void (*RecEvaluate) (BinaryTree_t* , Node_t*)){
    UNUSED(myTree);
    UNUSED(CurrentNode);
    UNUSED(RecEvaluate);
    fprintf(FileProc,   "\npop rbx      ; or\n"
                        "pop rax\n"
                        "cmp rax, 0\n"
                        "je  .L%d_1\n"
                        "cmp rbx, 0\n"
                        "je  .L%d_1\n"
                        "mov rax, %d\n"
                        "jmp .L%d_2\n"
                        ".L%d_1:\n"
                        "mov rax, 0\n"
                        ".L%d_2:\n"
                        "movzx rax, al  ; expand al to rax\n"
                        "push rax\n", label_counter,label_counter, INACCURACY, label_counter,label_counter,label_counter);   
    return 1;
}

//=======================================================================================

int _ABO(BinaryTree_t* myTree, FILE* FileProc, Node_t* CurrentNode , int label_counter, void (*RecEvaluate) (BinaryTree_t* , Node_t*)){
    UNUSED(myTree);
    UNUSED(CurrentNode);
    UNUSED(label_counter);
    UNUSED(RecEvaluate);
    fprintf(FileProc,   "\npop rbx      ; above\n"
                        "pop rax\n"
                        "cmp rax, rbx\n"
                        "setg  al       ; set func 1 or 0\n"
                        "movzx rax, al  ; expand al to rax\n"
                        "mov rbx, %d\n"
                        "mul rbx\n"
                        "push rax\n", INACCURACY);
    return 0;
}
int _BEL(BinaryTree_t* myTree, FILE* FileProc, Node_t* CurrentNode , int label_counter, void (*RecEvaluate) (BinaryTree_t* , Node_t*)){
    UNUSED(myTree);
    UNUSED(CurrentNode);
    UNUSED(label_counter);
    UNUSED(RecEvaluate);
    fprintf(FileProc,   "\npop rbx      ; below\n"
                        "pop rax\n"
                        "cmp rax, rbx\n"
                        "setl  al       ; set func 1 or 0\n"
                        "movzx rax, al  ; expand al to rax\n"
                        "mov rbx, %d\n"
                        "mul rbx\n"
                        "push rax\n", INACCURACY);
    return 0;
}
int _ABOEQU(BinaryTree_t* myTree, FILE* FileProc, Node_t* CurrentNode , int label_counter, void (*RecEvaluate) (BinaryTree_t* , Node_t*)){
    UNUSED(myTree);
    UNUSED(CurrentNode);
    UNUSED(label_counter);
    UNUSED(RecEvaluate);
    fprintf(FileProc,   "\npop rbx      ; above or equal\n"
                        "pop rax\n"
                        "cmp rax, rbx\n"
                        "setge al      ; set func 1 or 0\n"
                        "movzx rax, al  ; expand al to rax\n"
                        "mov rbx, %d\n"
                        "mul rbx\n"
                        "push rax\n", INACCURACY);
    return 0;
}
int _BELEQU(BinaryTree_t* myTree, FILE* FileProc, Node_t* CurrentNode , int label_counter, void (*RecEvaluate) (BinaryTree_t* , Node_t*)){
    UNUSED(myTree);
    UNUSED(CurrentNode);
    UNUSED(label_counter);
    UNUSED(RecEvaluate);
    fprintf(FileProc,   "\npop rbx      ; below or equal\n"
                        "pop rax\n"
                        "cmp rax, rbx\n"
                        "setle al      ; set func 1 or 0\n"
                        "movzx rax, al  ; expand al to rax\n"
                        "mov rbx, %d\n"
                        "mul rbx\n"
                        "push rax\n", INACCURACY);
    return 0;
}
int _NEQU(BinaryTree_t* myTree, FILE* FileProc, Node_t* CurrentNode , int label_counter, void (*RecEvaluate) (BinaryTree_t* , Node_t*)){
    UNUSED(myTree);
    UNUSED(CurrentNode);
    UNUSED(label_counter);
    UNUSED(RecEvaluate);
    fprintf(FileProc,   "\npop rbx      ; not equal\n"
                        "pop rax\n"
                        "cmp rax, rbx\n"
                        "setne al\n     ; set func 1 or 0\n"
                        "movzx rax, al  ; expand al to rax\n"
                        "mov rbx, %d\n"
                        "mul rbx\n"
                        "push rax\n", INACCURACY);
    return 0;
}
int _EQU(BinaryTree_t* myTree, FILE* FileProc, Node_t* CurrentNode , int label_counter, void (*RecEvaluate) (BinaryTree_t* , Node_t*)){
    UNUSED(myTree);
    UNUSED(CurrentNode);
    UNUSED(label_counter);
    UNUSED(RecEvaluate);
    fprintf(FileProc,   "\npop rbx      ; equal\n"
                        "pop rax\n"
                        "cmp rax, rbx\n"
                        "sete  al\n     ; set func 1 or 0\n"
                        "movzx rax, al  ; expand al to rax\n"
                        "mov rbx, %d\n"
                        "mul rbx\n"
                        "push rax\n", INACCURACY);
    return 0;
}
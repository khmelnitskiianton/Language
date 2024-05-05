#include <stdio.h>
#include <string.h>

#include "colors.h"
#include "tree.h"
#include "Stacks/stack_int.h"
#include "MyLangConfig.h"
#include "translator.h"
#include "log.h"
#include "output.h"
#include "creator.h"
#include "myassert.h"
#include "verificator.h"
#include "dsl.h"

static void     WriteStart      (void);
static void     WriteRecTree    (BinaryTree_t* myTree, Node_t* CurrentNode, StackInt_t* StackWhileCond);
static void     WriteDefFunc    (BinaryTree_t* myTree, Node_t* CurrentNode, StackInt_t* StackWhileCond);
static void     CountArgs       (BinaryTree_t* myTree, int* counter_args, Node_t* CurrentNode);

static void     WriteFuncLocalVars      (BinaryTree_t* myTree, Node_t* CurrentNode);
static void     WriteFuncArgVars        (BinaryTree_t* myTree, Node_t* CurrentNode);
static void     MoveToLocalVars         (BinaryTree_t* myTree, Node_t* Var);
static void     MoveToArgVars           (BinaryTree_t* myTree, Node_t* Var);
static void     CleanCountArgs          (BinaryTree_t* myTree);
static void     CleanFuncLocalVars      (void);
static void     CleanFuncArgVars        (void);

static void     WriteCall       (BinaryTree_t* myTree, Node_t* CurrentNode);
static void     WriteVar        (BinaryTree_t* myTree, Node_t* CurrentNode);
static void     WriteWhile      (BinaryTree_t* myTree, Node_t* CurrentNode, StackInt_t* StackWhileCond);
static void     GetArgs         (BinaryTree_t* myTree, Node_t* CurrentNode);
static void     WriteCond       (BinaryTree_t* myTree, Node_t* CurrentNode, StackInt_t* StackWhileCond);
static void     WriteCondCond   (BinaryTree_t* myTree, Node_t* CurrentNode, int curr_counter);
static void     WriteWhileCond  (BinaryTree_t* myTree, Node_t* CurrentNode, int curr_counter);
static void     WriteReturn     (BinaryTree_t* myTree, Node_t* CurrentNode);

static int      FindIndexLocalVar   (BinaryTree_t* myTree, Node_t* CurrentNode);
static int      FindIndexArgVar     (BinaryTree_t* myTree, Node_t* CurrentNode);

static void     WriteBreak      (BinaryTree_t* myTree, Node_t* CurrentNode, StackInt_t* StackWhileCond);
static void     WriteContinue   (BinaryTree_t* myTree, Node_t* CurrentNode, StackInt_t* StackWhileCond);

static char     buffer_file[SIZE_OF_OUT_PATH]       = {};
static Var_t    FuncArgVars[SIZE_OF_VARIABLES]     = {};
static Var_t    FuncLocalVars[SIZE_OF_VARIABLES]    = {};
static Var_t    FuncGlobalVars[SIZE_OF_VARIABLES]   = {};   //add global vars
static FILE*    FileProc = NULL;

static const int DIMENSION = 8;

static int  counter_while = 0;
static int  counter_if    = 0;
static bool is_void       = false;  
static int  counter_move_vars = 0; 

EnumOfErrors TranslateTree(BinaryTree_t* myTree, const char* in_file_path, StackInt_t* StackWhileCond)
{
    MYASSERT(myTree, ERR_BAD_POINTER_PASSED_IN_FUNC, return ERR_BAD_POINTER_PASSED_IN_FUNC)
    //Create File
    size_t position = strlen(in_file_path)+1;
    char buffer_name_file[SIZE_OF_NAME_FILE] = {};
    while ((position > 0) && (in_file_path[position] != '.'))
    {
        position--;
    }
    if (!position)
    {
        position = strlen(in_file_path);
    }
    for (size_t i = 0; i < position; i++)
    {
        buffer_name_file[i] = in_file_path[i];    
    }
    //In buffer_name_file - name of file
    snprintf(buffer_file, SIZE_OF_OUT_PATH, "%s%s", buffer_name_file, EXT_PROC);
    char buffer_create[SIZE_OF_OUT_COMMAND] = {};
    snprintf(buffer_create, SIZE_OF_OUT_COMMAND, "touch %s",buffer_file);
    system(buffer_create);
    CleanCharBuffer(buffer_create, SIZE_OF_OUT_COMMAND); 
    fprintf(stdout, BLUE "[was created tree file: %s]\n" RESET, buffer_file);

    FileProc = OpenFile (buffer_file, "w");

    //Start Writing
    WriteStart();
    //WriteGlobalVars(); for future
    WriteRecTree(myTree, myTree->Root, StackWhileCond);

    return ERR_OK;
}

static void WriteStart(void)
{
    fprintf(FileProc,   ";#============#\n"
                        ";#Start module#\n"
                        ";#============#\n\n"
                        "global _start\n\n"
                        "section .text\n\n"
                        "_stack_offset equ 8\n"
                        "\n_start:\n"
                        "\tcall main\n"
                        "\tmov rax, 60\n"
                        "\tmov rdi, 0\n"
                        "\tsyscall\n\n"
                        ";#===========#\n"
                        ";#Main module#\n"
                        ";#===========#\n\n");
    //TODO: extern in out functions.
}

static void WriteRecTree(BinaryTree_t* myTree, Node_t* CurrentNode, StackInt_t* StackWhileCond)
{
    if (!CurrentNode) return;

    if (CurrentNode->Type == OPERATOR)
    {
        if (ArrayOperators[CurrentNode->Value.Index].Class == DIVIDER)
        {
            WriteRecTree(myTree, L, StackWhileCond);
            WriteRecTree(myTree, R, StackWhileCond);
        }
        switch (ArrayOperators[CurrentNode->Value.Index].Class)
        {
            case FUNC_DEF: //done
                WriteDefFunc(myTree, CurrentNode, StackWhileCond);
            break;
            case CALL:  //done
                WriteCall(myTree, CurrentNode);
            break;
            case RETURN: //done
                WriteReturn(myTree, CurrentNode);
            break;
            case VAR:
                WriteVar(myTree, CurrentNode);
            break;
            case WHILE: //done
                WriteWhile(myTree, CurrentNode, StackWhileCond);
            break;
            case IF: //done
                WriteCond(myTree, CurrentNode, StackWhileCond);
            break;
            case BREAK: //done
                WriteBreak(myTree, CurrentNode, StackWhileCond);
            break;
            case CONTINUE: //done
                WriteContinue(myTree, CurrentNode, StackWhileCond);
            break;
            //all cases
            default: break;
        }
    }
    else 
    {
        MYASSERT(0, ERR_BAD_TRANSLATION, return)
    }

    return;
}

//amount of regs return value in rax
static void WriteDefFunc(BinaryTree_t* myTree, Node_t* CurrentNode, StackInt_t* StackWhileCond)
{ 
    if (ArrayOperators[CurrentNode->Left->Left->Value.Index].Class == VOID)
    {
        is_void = true;
    }
    else
    {
        is_void = false;
    }
    //Vars with args 
    int counter_args        = 0;
    int counter_local_vars  = 0;
    CountArgs(myTree, &counter_args, C->Right->Left);
    CountArgs(myTree, &counter_local_vars, C->Right->Right);
    fprintf(FileProc, "%s:\n", myTree->Variables[CurrentNode->Left->Right->Value.Index].Name);
    fprintf(FileProc,   "push rbp\n"
                        "mov  rbp, rsp\n"
                        "sub  rsp, %d\n\n", counter_local_vars * DIMENSION);
    counter_move_vars = 0;
    WriteFuncArgVars(myTree, CurrentNode->Right->Left);
    counter_move_vars = 0;
    WriteFuncLocalVars(myTree, CurrentNode->Right->Right);
    WriteRecTree(myTree, CurrentNode->Right->Right, StackWhileCond);
    fprintf(FileProc,   "\npop  rbp\n"
                        "ret\n\n");
    CleanCountArgs(myTree);                    
    CleanFuncLocalVars();
    CleanFuncArgVars();
}

//done
static void CountArgs(BinaryTree_t* myTree, int* counter_args, Node_t* CurrentNode)
{
    if (!C) return;
    if ((C->Type == OPERATOR) && (ArrayOperators[C->Value.Index].Class == VAR)) 
    {
        if (R->Type == VARIABLE)
        {
            if (myTree->Variables[(R)->Value.Index].Number == 0)  //for checking repeats
            {
                (*counter_args)++;
                myTree->Variables[(R)->Value.Index].Number = 1;
            }
        }
        else
        {
            if (myTree->Variables[(R)->Left->Value.Index].Number == 0)  //for checking repeats
            {
                (*counter_args)++;
                myTree->Variables[(R)->Left->Value.Index].Number = 1;
            }
        }
        return;
    }
    CountArgs(myTree, counter_args, L); //true calling
    CountArgs(myTree, counter_args, R);
}

static void CleanCountArgs(BinaryTree_t* myTree)
{
    for (size_t i = 0; i < SIZE_OF_VARIABLES; i++)
    {
        myTree->Variables[i].Number = 0;
    }
}

//done
static void WriteFuncLocalVars(BinaryTree_t* myTree, Node_t* CurrentNode)
{
    if (!C) return ;
    if ((C->Type == OPERATOR) && (ArrayOperators[C->Value.Index].Class == VAR)) 
    {
        MoveToLocalVars(myTree, R);
        return;
    }
    WriteFuncLocalVars(myTree, L); //true calling
    WriteFuncLocalVars(myTree, R);
}
//done
static void MoveToLocalVars(BinaryTree_t* myTree, Node_t* Var)
{
    int free_place = 0;
    for (size_t i = 0; i < SIZE_OF_VARIABLES; i++)
    {
        if (FuncLocalVars[i].Number == -1) 
        {
            free_place = int(i);
            break;
        }
    }
    strncpy(FuncLocalVars[free_place].Name, myTree->Variables[Var->Value.Index].Name, SIZE_OF_VAR);
    FuncLocalVars[free_place].Number = counter_move_vars;
    fprintf(FileProc, "mov qword [rbp - _stack_offset*%d], 0\n", counter_move_vars); //for init
    counter_move_vars++;
}
//done
static void WriteFuncArgVars(BinaryTree_t* myTree, Node_t* CurrentNode)
{
    if (!C) return ;
    if ((C->Type == OPERATOR) && (ArrayOperators[C->Value.Index].Class == VAR)) 
    {
        MoveToArgVars(myTree, R);
        return;
    }
    WriteFuncArgVars(myTree, L); //true calling
    WriteFuncArgVars(myTree, R);
}
//done
static void MoveToArgVars(BinaryTree_t* myTree, Node_t* Var)
{
    int free_place = 0;
    for (size_t i = 0; i < SIZE_OF_VARIABLES; i++)
    {
        if (FuncArgVars[i].Number == -1) 
        {
            free_place = int(i);
            break;
        }
    }
    strncpy(FuncArgVars[free_place].Name, myTree->Variables[Var->Value.Index].Name, SIZE_OF_VAR);
    FuncArgVars[free_place].Number = counter_move_vars;
    counter_move_vars++;
}
//done
static void CleanFuncLocalVars(void)
{
    for (size_t i = 0; i < SIZE_OF_VARIABLES; i++)
    {
        CleanCharBuffer(FuncLocalVars[i].Name, SIZE_OF_VAR);
        FuncLocalVars[i].Number = -1;
    }
}
static void CleanFuncArgVars(void)
{
    for (size_t i = 0; i < SIZE_OF_VARIABLES; i++)
    {
        CleanCharBuffer(FuncArgVars[i].Name, SIZE_OF_VAR);
        FuncArgVars[i].Number = -1;
    }
}

static void WriteVar(BinaryTree_t* myTree, Node_t* CurrentNode)
{

}

//done
static void WriteCall(BinaryTree_t* myTree, Node_t* CurrentNode)
{
    GetArgs(myTree, R);
    char* name_of_func = myTree->Variables[L->Value.Index].Name;
    fprintf(FileProc, "call %s\n\n", name_of_func);
}
//done
static void GetArgs(BinaryTree_t* myTree, Node_t* CurrentNode)
{
    if (!CurrentNode) return;
    if (CurrentNode->Type == OPERATOR)
    {
        GetArgs(myTree, L);
        GetArgs(myTree, R);
        return;
    }
    if (CurrentNode->Type == NUMBER)
    {
        fprintf(FileProc, "push %g\n", CurrentNode->Value.Number);       
        return; 
    }
    if (CurrentNode->Type == VARIABLE)
    {
        int index_call_var = FindIndexLocalVar(myTree, C);
        if (index_call_var != -1)
        {
            fprintf(FileProc, "push qword [rbp - _stack_offset*%d]\n", FuncLocalVars[index_call_var].Number);
        }
        index_call_var = FindIndexArgVar(myTree, C);
        if (index_call_var != -1)
        {
            fprintf(FileProc, "push qword [rbp + _stack_offset*%d]\n", FuncArgVars[index_call_var].Number);
        }
        return;
    }
    MYASSERT(0, ERR_BAD_END_FUNC, )
}
//done
static void WriteReturn(BinaryTree_t* myTree, Node_t* CurrentNode)
{
    int index_ret_var = 0;
    if (is_void) {fprintf(FileProc, "\npop  rbp\n"
                                    "ret\n\n");}
    else 
    {
        index_ret_var = FindIndexLocalVar(myTree, L);
        if (index_ret_var != -1)
        {
            fprintf(FileProc, "\nmov rax, qword [rbp - _stack_offset*%d]\npop  rbp\nret\n", FuncLocalVars[index_ret_var].Number);
        }
        index_ret_var = FindIndexArgVar(myTree, L);
        if (index_ret_var != -1)
        {
            fprintf(FileProc, "\nmov rax, qword [rbp + _stack_offset*%d]\npop  rbp\nret\n", FuncArgVars[index_ret_var].Number);
        }
    }
}
//done
static int FindIndexArgVar(BinaryTree_t* myTree, Node_t* CurrentNode)
{
    for (size_t i = 0; i < SIZE_OF_VARIABLES; i++)
    {
        //printf("Arg: 1: %s 2: %s\n", FuncLocalVars[i].Name, myTree->Variables[C->Value.Index].Name);
        if (!strncmp(FuncArgVars[i].Name, myTree->Variables[C->Value.Index].Name, SIZE_OF_VAR))
        {
            return (int)i;
        }
    }
    return -1;
}
static int FindIndexLocalVar(BinaryTree_t* myTree, Node_t* CurrentNode)
{
    for (size_t i = 0; i < SIZE_OF_VARIABLES; i++)
    {
        //printf("Local: 1: %s 2: %s\n", FuncLocalVars[i].Name, myTree->Variables[C->Value.Index].Name);
        if (!strncmp(FuncLocalVars[i].Name, myTree->Variables[C->Value.Index].Name, SIZE_OF_VAR))
        {
            return (int)i;
        }
    }
    return -1;
}

//=======================================================================================
//=======================================================================================

//done
static void WriteWhile(BinaryTree_t* myTree, Node_t* CurrentNode, StackInt_t* StackWhileCond)
{
    fprintf(FileProc, "\njmp while_check_%d\n", counter_while);
    fprintf(FileProc, ".while_start_%d:\n\n", counter_while);
    push_int(StackWhileCond, counter_while);    //save
    push_int(StackWhileCond, WHILE_TYPE);
    counter_while++;
    if (R) {WriteRecTree(myTree, R, StackWhileCond);} //body
    int save_counter  = 0;
    int type_operator = 0;
    pop_int(StackWhileCond, &type_operator);
    pop_int(StackWhileCond, &save_counter);     //pop index
    fprintf(FileProc, "\n.while_check_%d:\n", save_counter);
    //Condition
    WriteWhileCond(myTree, L, save_counter);
    fprintf(FileProc, ".while_end___%d:\n\n", save_counter);
}

//done
static void WriteWhileCond(BinaryTree_t* myTree, Node_t* CurrentNode, int curr_counter)
{
    if (!C) 
    {
        fprintf(FileProc, "jmp while_start_%d\n", curr_counter);
        return;
    }
    if (C->Type == NUMBER)
    {
        if (!Compare(C->Value.Number, 0))
        {
            fprintf(FileProc, "jmp while_start_%d\n", curr_counter);
        }
        return;
    }
    fprintf(FileProc,   "mov rax, %g\n"
                        "mov rbx, %g\n"
                        "cmp rax, rbx\n", R->Value.Number, L->Value.Number);
    switch (ArrayOperators[CurrentNode->Value.Index].Id)
    {
        case 30://>
            fprintf(FileProc, "ja while_start_%d\n", curr_counter);
        break; 
        case 31://<
            fprintf(FileProc, "jb while_start_%d\n", curr_counter);
        break; 
        case 32://>=
            fprintf(FileProc, "jae while_start_%d\n", curr_counter);
        break; 
        case 33://<=
            fprintf(FileProc, "jbe while_start_%d\n", curr_counter);
        break; 
        case 34://!=
            fprintf(FileProc, "jne while_start_%d\n", curr_counter);
        break; 
        case 35://==
            fprintf(FileProc, "je while_start_%d\n", curr_counter);
        break; 
        default: break;
    }   
}

//done
static void WriteCond(BinaryTree_t* myTree, Node_t* CurrentNode,StackInt_t* StackWhileCond)
{
    fprintf(FileProc, "\njmp if_check_%d\n", counter_if);
    fprintf(FileProc, "if_start_%d:\n", counter_if);
    push_int(StackWhileCond, counter_if);    //save
    push_int(StackWhileCond, IF_TYPE);
    counter_if++;
    //body
    if ((R) && (R->Type == OPERATOR)&&(ArrayOperators[R->Value.Index].Class == ELSE))
    {
        WriteRecTree(myTree, (R)->Left, StackWhileCond);
        int save_counter  = 0;
        int type_operator = 0;
        pop_int(StackWhileCond, &type_operator);
        pop_int(StackWhileCond, &save_counter);     //pop index
        fprintf(FileProc, "jmp if_end_%d\n", save_counter);
        fprintf(FileProc, "if_check_%d:\n",  save_counter);
        //Condition
        WriteCondCond(myTree, L, save_counter);
        WriteRecTree(myTree, (R)->Right, StackWhileCond);
        fprintf(FileProc, "if_end___%d:\n\n", save_counter);
    }
    else
    {
        WriteRecTree(myTree, R, StackWhileCond);
        int save_counter = 0;
        int type_operator = 0;
        pop_int(StackWhileCond, &type_operator);
        pop_int(StackWhileCond, &save_counter);     //pop index
        fprintf(FileProc, "jmp if_end_%d\n", save_counter);
        fprintf(FileProc, "if_check_%d:\n",  save_counter);
        //Condition
        WriteCondCond(myTree, L, save_counter);
        fprintf(FileProc, "if_end___%d:\n\n", save_counter);
    }
}

//done
static void WriteCondCond(BinaryTree_t* myTree, Node_t* CurrentNode, int curr_counter)
{
    if (!C) 
    {
        fprintf(FileProc, "jmp if_start_%d", curr_counter);
        return;
    }
    if (C->Type == NUMBER)
    {
        if (!Compare(C->Value.Number, 0))
        {
            fprintf(FileProc, "jmp if_start_%d\n", curr_counter);
        }
        return;
    }
    fprintf(FileProc,   "mov rax, %g\n"
                        "mov rbx, %g\n"
                        "cmp rax, rbx\n", R->Value.Number, L->Value.Number);
    switch (ArrayOperators[CurrentNode->Value.Index].Id)
    {
        case 30://>
            fprintf(FileProc, "ja if_start_%d\n", curr_counter);
        break; 
        case 31://<
            fprintf(FileProc, "jb if_start_%d\n", curr_counter);
        break; 
        case 32://>=
            fprintf(FileProc, "jae if_start_%d\n", curr_counter);
        break; 
        case 33://<=
            fprintf(FileProc, "jbe if_start_%d\n", curr_counter);
        break; 
        case 34://!=
            fprintf(FileProc, "jne if_start_%d\n", curr_counter);
        break; 
        case 35://==
            fprintf(FileProc, "je if_start_%d\n", curr_counter);
        break; 
        default: break; //TODO: 
    }   
}

//done
static void WriteBreak(BinaryTree_t* myTree, Node_t* CurrentNode, StackInt_t* StackWhileCond)
{
    bool flag = true;
    for (int i = (StackWhileCond->size - 1); i >= 0; i--)
    {
        if (StackWhileCond->data[i] == WHILE_TYPE)
        {
            flag = false;
            fprintf(FileProc, "jmp while_end___%d\n", StackWhileCond->data[i-1]);
            break;
        }
    }
    if (flag)
    {
        USER_ERROR(0, ERR_BAD_USING_BREAK_CONT, "", return)
    }
}

//done
static void WriteContinue(BinaryTree_t* myTree, Node_t* CurrentNode, StackInt_t* StackWhileCond)
{
    bool flag = true; 
    for (int i = (StackWhileCond->size - 1); i >= 0; i--)
    {
        if (StackWhileCond->data[i] == WHILE_TYPE)
        {
            flag = false;
            fprintf(FileProc, "jmp while_start___%d\n", StackWhileCond->data[i-1]);
            break;
        }
    }
    if (flag)
    {
        USER_ERROR(0, ERR_BAD_USING_BREAK_CONT, "", return)
    }
}
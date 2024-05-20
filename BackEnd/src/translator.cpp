#include <stdio.h>
#include <string.h>

#include "colors.h"
#include "tree.h"
#include "Stacks/stack_int.h"
#include "MyLangConfig/MyLangConfig.h"
#include "translator.h"
#include "log.h"
#include "output.h"
#include "creator.h"
#include "myassert.h"
#include "verificator.h"
#include "dsl.h"

static void     WriteAsmHeader  (void);
static void     WriteRecTree    (BinaryTree_t* myTree, Node_t* CurrentNode, StackInt_t* StackWhileCond);
static void     WriteDefFunc    (BinaryTree_t* myTree, Node_t* CurrentNode, StackInt_t* StackWhileCond);
static void     CountArgs       (BinaryTree_t* myTree, int* counter_args, Node_t* CurrentNode);

static void     WriteFuncLocalVars      (BinaryTree_t* myTree, Node_t* CurrentNode);
static void     WriteFuncArgVars        (BinaryTree_t* myTree, Node_t* CurrentNode);
static void     MoveToLocalVars         (BinaryTree_t* myTree, Node_t* CurrentNode);
static void     MoveToArgVars           (BinaryTree_t* myTree, Node_t* CurrentNode);
static void     CleanCountArgs          (BinaryTree_t* myTree);
static void     CleanGlobalVarArrays    (void);

static void     WriteCall               (BinaryTree_t* myTree, Node_t* CurrentNode);
static void     WriteVar                (BinaryTree_t* myTree, Node_t* CurrentNode);
static void     WriteWhile              (BinaryTree_t* myTree, Node_t* CurrentNode, StackInt_t* StackWhileCond);
static void     GetArgs                 (BinaryTree_t* myTree, Node_t* CurrentNode);
static void     WriteIf                 (BinaryTree_t* myTree, Node_t* CurrentNode, StackInt_t* StackWhileCond);
static void     WriteIfCondition        (BinaryTree_t* myTree, Node_t* CurrentNode, int curr_counter);
static void     WriteWhileCondition     (BinaryTree_t* myTree, Node_t* CurrentNode, int curr_counter);
static void     WriteReturn             (BinaryTree_t* myTree, Node_t* CurrentNode);

static int      FindIndexInVarArray     (BinaryTree_t* myTree, Node_t* CurrentNode, Var_t* ArrayOfVars);

static void     WriteBreak      (BinaryTree_t* myTree, Node_t* CurrentNode, StackInt_t* StackWhileCond);
static void     WriteContinue   (BinaryTree_t* myTree, Node_t* CurrentNode, StackInt_t* StackWhileCond);

static int      NotInitCheck    (BinaryTree_t* myTree, Node_t* CurrentNode);

static char     buffer_file     [SIZE_OF_OUT_PATH]  = {};
static Var_t    FuncArgVars     [SIZE_OF_VARIABLES] = {};
static Var_t    FuncLocalVars   [SIZE_OF_VARIABLES] = {};

//TODO:add global vars

static FILE*    FileProc = NULL;

static const int STACK_VAR_SIZE = 8;
static const int ADDRESS_CALL   = 16;
static const int ADDRESS_RBP    = 8;

static int      counter_while       = 0;
static int      counter_if          = 0;
static int      counter_label_var   = 0;
static int      counter_move_vars   = 0;
static int      counter_push_args   = 0;

static bool             is_void     = false;   
static BinaryTree_t*    ptr_of_tree = NULL;

EnumOfErrors TranslateTree(BinaryTree_t* myTree, const char* in_file_path, StackInt_t* StackWhileCond)
{
    MYASSERT(myTree, ERR_BAD_POINTER_PASSED_IN_FUNC, return ERR_BAD_POINTER_PASSED_IN_FUNC)
    
    //Create File
    ptr_of_tree = myTree;                       //take name of file without extension
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
    WriteAsmHeader();
    //WriteGlobalVars(); for future
    WriteRecTree(myTree, myTree->Root, StackWhileCond);

    return ERR_OK;
}

static void WriteAsmHeader(void)
{
    fprintf(FileProc,   ";#=========================#\n"
                        ";# THIS FILE WAS GENERATED #\n"
                        ";#    BY ANTON'S LANGUAGE  #\n"
                        ";#GITHUB: khmelnitskiianton#\n"
                        ";#=========================#\n"
                        "\n"
                        ";#=========================#\n"
                        ";#      Start module       #\n"
                        ";#=========================#\n"
                        "_stack_offset equ 8\n"
                        "\n"
                        INOUT_FUNCTIONS
                        "\n");
    fprintf(FileProc,   
                        ";#=========================#\n"
                        ";#      Main module        #\n"
                        ";#=========================#\n\n");
}

static void WriteRecTree(BinaryTree_t* myTree, Node_t* CurrentNode, StackInt_t* StackWhileCond)
{
    if (!C) return;

    if (CHECK_TYPE_OPER(CurrentNode))
    {
        if (CHECK_OPER_CLASS(CurrentNode, DIVIDER))
        {
            WriteRecTree(myTree, L, StackWhileCond);
            WriteRecTree(myTree, R, StackWhileCond);
        }
        switch (ArrayOperators[CurrentNode->Value.Index].Class)
        {
            case FUNC_DEF:
                WriteDefFunc(myTree, CurrentNode, StackWhileCond);
            break;
            case CALL:
                WriteCall(myTree, CurrentNode);
            break;
            case RETURN:
                WriteReturn(myTree, CurrentNode);
            break;
            case VAR:
                WriteVar(myTree, CurrentNode);
            break;
            case WHILE:
                WriteWhile(myTree, CurrentNode, StackWhileCond);
            break;
            case IF:
                WriteIf(myTree, CurrentNode, StackWhileCond);
            break;
            case BREAK:
                WriteBreak(myTree, CurrentNode, StackWhileCond);
            break;
            case CONTINUE:
                WriteContinue(myTree, CurrentNode, StackWhileCond);
            break;
            //all cases for remove [-Wswitch-enum]
            case ZERO:          [[fallthrough]];
            case FIRST:         [[fallthrough]];
            case SECOND:        [[fallthrough]];
            case THIRD:         [[fallthrough]];
            case FOURTH:        [[fallthrough]];
            case EQUAL:         [[fallthrough]];
            case DIVIDER:       [[fallthrough]];
            case DIVIDER_ARG:   [[fallthrough]];
            case OP_BR_ONE:     [[fallthrough]];
            case CL_BR_ONE:     [[fallthrough]];
            case OP_BR_TWO:     [[fallthrough]];
            case CL_BR_TWO:     [[fallthrough]];
            case OP_BR_THREE:   [[fallthrough]];
            case CL_BR_THREE:   [[fallthrough]];
            case VOID:          [[fallthrough]];
            case ELSE:          [[fallthrough]];
            case FUNC_DEF_HELP: [[fallthrough]];
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
    if (CHECK_OPER_CLASS(CurrentNode->Left->Left, VOID))
    {
        is_void = true;
    }
    else
    {
        is_void = false;
    }

    //Clean all global vars before translation new function
    counter_if          = 0;
    counter_while       = 0;
    counter_label_var   = 0;
    counter_move_vars   = 0;
    CleanGlobalVarArrays();
    CleanCountArgs(myTree);

    int counter_args        = 0;
    int counter_local_vars  = 0;
    CountArgs(myTree, &counter_args, C->Right->Left);
    CountArgs(myTree, &counter_local_vars, C->Right->Right);
    
    fprintf(FileProc,   "\n;#=========Function========#\n"
                        "%s:\n"
                        ";#=======Input=Action======#\n"
                        "push rbp\n"
                        "mov  rbp, rsp\n"
                        "sub  rsp, %d\n"
                        ";#=======End=Action========#\n"
                        "\n;#========Init=Local=======#\n",
                        myTree->Variables[L->Right->Value.Index].Name, counter_local_vars * STACK_VAR_SIZE);
    
    counter_move_vars = 0;
    WriteFuncArgVars(myTree, R->Left);
    counter_move_vars = 0;
    WriteFuncLocalVars(myTree, R->Right);

    fprintf(FileProc,   ";#========End=Init=========#\n\n");
    WriteRecTree(myTree, R->Right, StackWhileCond);
    fprintf(FileProc,   "\n;#=======Leave=Action======#\n"   
                        "mov  rsp, rbp\n"
                        "pop  rbp\n"
                        "ret\n"
                        ";#=======End=Function======#\n");
}

//done
static void CountArgs(BinaryTree_t* myTree, int* counter_args, Node_t* CurrentNode)
{
    if (!C) return;
    if ((CHECK_TYPE_OPER(CurrentNode)) && (CHECK_OPER_CLASS(CurrentNode, VAR))) 
    {
        if (CHECK_TYPE_VAR(CurrentNode->Right))
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

static void WriteFuncLocalVars(BinaryTree_t* myTree, Node_t* CurrentNode)
{
    if (!C) return ;
    if ((CHECK_TYPE_OPER(CurrentNode)) && (CHECK_OPER_CLASS(CurrentNode, VAR))) 
    {
        if (CHECK_TYPE_VAR(CurrentNode->Right)) MoveToLocalVars(myTree, R);
        else MoveToLocalVars(myTree, R->Left);
        return;
    }
    WriteFuncLocalVars(myTree, L); //true calling
    WriteFuncLocalVars(myTree, R);
}

static void MoveToLocalVars(BinaryTree_t* myTree, Node_t* CurrentNode)
{
    int free_place = 0;
    for (size_t i = 0; i < SIZE_OF_VARIABLES; i++)
    {
        if (!strncmp(FuncLocalVars[i].Name, myTree->Variables[C->Value.Index].Name, SIZE_OF_VAR))
        {
            return; //check for collisions
        }
        if (FuncLocalVars[i].Number == -1) 
        {
            free_place = int(i);
            break;
        }
    }
    strncpy(FuncLocalVars[free_place].Name, myTree->Variables[C->Value.Index].Name, SIZE_OF_VAR);
    FuncLocalVars[free_place].Number = counter_move_vars;
    fprintf(FileProc, "mov qword [rbp - _stack_offset*%d - %d], 0\n", counter_move_vars, ADDRESS_RBP); //for init
    counter_move_vars++;
}
//done
static void WriteFuncArgVars(BinaryTree_t* myTree, Node_t* CurrentNode)
{
    if (!C) return ;
    if ((CHECK_TYPE_OPER(CurrentNode)) && (CHECK_OPER_CLASS(CurrentNode, VAR))) 
    {
        MoveToArgVars(myTree, R);
        return;
    }
    WriteFuncArgVars(myTree, L); //true calling
    WriteFuncArgVars(myTree, R);
}
//done
static void MoveToArgVars(BinaryTree_t* myTree, Node_t* CurrentNode)
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
    strncpy(FuncArgVars[free_place].Name, myTree->Variables[C->Value.Index].Name, SIZE_OF_VAR);
    FuncArgVars[free_place].Number = counter_move_vars;
    counter_move_vars++;
}

static void CleanGlobalVarArrays(void)
{
    for (size_t i = 0; i < SIZE_OF_VARIABLES; i++)
    {
        CleanCharBuffer(FuncLocalVars[i].Name, SIZE_OF_VAR);
        FuncLocalVars[i].Number = -1;

        CleanCharBuffer(FuncArgVars[i].Name, SIZE_OF_VAR);
        FuncArgVars[i].Number = -1;
    }
}

static void WriteVar(BinaryTree_t* myTree, Node_t* CurrentNode)
{
    fprintf(FileProc,   "\n;#========Var=Assign=======#\n");
    
    if (NotInitCheck(myTree, C)) return;

    RecEvaluate(myTree, R->Right);
    
    fprintf(FileProc,   "\n;assign\n"
                        "pop rax\n");
    
    int index_assign_var  = FindIndexInVarArray(myTree, R->Left, FuncLocalVars);
    if (index_assign_var != -1)
    {
        fprintf(FileProc, "mov qword [rbp - _stack_offset*%d - %d], rax\n", FuncLocalVars[index_assign_var].Number, ADDRESS_RBP);
        fprintf(FileProc, ";#=========End=Var=========#\n");
        return;
    }
    USER_ERROR(0, ERR_UNKNOWN_VAR, "", return)
}

void RecEvaluate(BinaryTree_t* myTree, Node_t* CurrentNode)
{
    if (!C) return;
    if (CHECK_TYPE_NUM(CurrentNode))
    {
        fprintf(FileProc, "\npush %d\n", int((C->Value.Number)*INACCURACY));
        return;
    }
    if (CHECK_TYPE_VAR(CurrentNode))
    {
        int index_use_var = FindIndexInVarArray(myTree, C, FuncLocalVars);
        if (index_use_var != -1)
        {
            fprintf(FileProc, "\npush qword [rbp - _stack_offset*%d - %d]\n", FuncLocalVars[index_use_var].Number, ADDRESS_RBP);
            return;
        }
        index_use_var = FindIndexInVarArray(myTree, C, FuncArgVars);
        if (index_use_var != -1)
        {
            fprintf(FileProc, "\npush qword [rbp + _stack_offset*%d + %d]\n", FuncArgVars[index_use_var].Number, ADDRESS_CALL);
            return;
        }
    }    
    
    if ((CHECK_TYPE_OPER(CurrentNode)) && (!(CHECK_OPER_CLASS(CurrentNode, CALL))))
    {
        RecEvaluate(myTree, L);
        RecEvaluate(myTree, R);
    }

    if (CHECK_TYPE_OPER(CurrentNode))
    { 
        //Process unary operations
        if (CHECK_OPER_CLASS(CurrentNode, CALL))
        {
            WriteCall(myTree, C);
            fprintf(FileProc, "push rax\n");
            return;
        }
        if (ArrayOperators[C->Value.Index].Operation != NULL)
        {
            counter_label_var += ArrayOperators[C->Value.Index].Operation(myTree, FileProc, C, counter_label_var, RecEvaluate);
            return;
        }
        USER_ERROR(0, ERR_UNKNOWN_OPERATOR, "",return)
    }
    USER_ERROR(0, ERR_UNKNOWN_NODE, "",return)
}

static void WriteCall(BinaryTree_t* myTree, Node_t* CurrentNode)
{
    fprintf(FileProc,   ";#==========Call===========#\n");
    counter_push_args = 0;
    GetArgs(myTree, R);
    
    char* name_of_func = myTree->Variables[L->Value.Index].Name;
    
    fprintf(FileProc,   "\ncall %s\n"
                        "add rsp,%d\n"
                        ";#=========End=Call========#\n", name_of_func, counter_push_args * STACK_VAR_SIZE);

}

static void GetArgs(BinaryTree_t* myTree, Node_t* CurrentNode)
{
    if (!C) return;
    if ((CHECK_TYPE_OPER(CurrentNode)) && (CHECK_OPER_CLASS(CurrentNode, DIVIDER_ARG)))
    {
        GetArgs(myTree, R);     // cdecl convention reverse pushing args!
        GetArgs(myTree, L);
        return;
    }
    counter_push_args++;
    RecEvaluate(myTree, C);
}

static void WriteReturn(BinaryTree_t* myTree, Node_t* CurrentNode)
{
    if (is_void) 
    {
        fprintf(FileProc,   "\n;#=======Void=Return=======#\n"
                            "mov    rsp, rbp\n"
                            "pop    rbp\n"
                            "ret\n"
                            ";#=========End=Return========#\n\n");
        return;
    }
    USER_ERROR(L, ERR_RETURN_NOTHING_IN_VAR, "", return)
    
    fprintf(FileProc,   "\n;#========Var=Return=======#\n");
    
    RecEvaluate(myTree, L);

    fprintf(FileProc,   "\npop rax\n"
                        "mov rsp, rbp\n"
                        "pop  rbp\n"
                        "ret\n"
                        ";#=======End=Return======#\n");
}

static int FindIndexInVarArray(BinaryTree_t* myTree, Node_t* CurrentNode, Var_t* ArrayOfVars)
{
    for (size_t i = 0; i < SIZE_OF_VARIABLES; i++)
    {
        //printf("Arg: 1: %s 2: %s\n", FuncArgVars[i].Name, myTree->Variables[C->Value.Index].Name);
        if (!strncmp(ArrayOfVars[i].Name, myTree->Variables[C->Value.Index].Name, SIZE_OF_VAR))
        {
            return (int)i;
        }
    }
    return -1;
}
//=======================================================================================
//=======================================================================================

static void WriteWhile(BinaryTree_t* myTree, Node_t* CurrentNode, StackInt_t* StackWhileCond)
{
    fprintf(FileProc,   ";#===========While=========#\n"
                        "jmp .while_check_%d\n"
                        ".while_start_%d:\n\n", counter_while, counter_while);
    
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
    WriteWhileCondition(myTree, L, save_counter);
    fprintf(FileProc,   ".while_end_%d:\n\n"
                        ";#=======End=While========#\n", save_counter);
}

//done
static void WriteWhileCondition(BinaryTree_t* myTree, Node_t* CurrentNode, int curr_counter)
{
    if (!C) 
    {
        fprintf(FileProc, "jmp .while_start_%d\n", curr_counter);
        return;
    }
    fprintf(FileProc,   ";#========Condition========#\n");
    RecEvaluate(myTree, C);
    fprintf(FileProc,   "\npop rax\n"
                        "cmp rax, 0\n"
                        "jne .while_start_%d\n", curr_counter);
}

//done
static void WriteIf(BinaryTree_t* myTree, Node_t* CurrentNode,StackInt_t* StackWhileCond)
{
    fprintf(FileProc,   ";#=============If==========#\n"
                        "jmp .if_check_%d\n"
                        ".if_start_%d:\n", counter_if, counter_if);

    push_int(StackWhileCond, counter_if);    //save
    push_int(StackWhileCond, IF_TYPE);
    counter_if++;
    //body
    if ((R) && (CHECK_TYPE_OPER(CurrentNode->Right)) && (CHECK_OPER_CLASS(CurrentNode->Right, ELSE)))
    {
        WriteRecTree(myTree, (R)->Left, StackWhileCond);
        
        int save_counter  = 0;
        int type_operator = 0;
        pop_int(StackWhileCond, &type_operator);
        pop_int(StackWhileCond, &save_counter);     //pop index

        fprintf(FileProc,   "jmp .if_end_%d\n"
                            ".if_check_%d:\n", save_counter, save_counter);
        //Condition
        WriteIfCondition(myTree, L, save_counter);
        WriteRecTree(myTree, (R)->Right, StackWhileCond);
        fprintf(FileProc, ".if_end_%d:\n\n", save_counter);
    }
    else
    {
        WriteRecTree(myTree, R, StackWhileCond);
        
        int save_counter = 0;
        int type_operator = 0;
        pop_int(StackWhileCond, &type_operator);
        pop_int(StackWhileCond, &save_counter);     //pop index

        fprintf(FileProc,   "jmp .if_end_%d\n"
                            ".if_check_%d:\n", save_counter, save_counter);
        //Condition
        WriteIfCondition(myTree, L, save_counter);
        fprintf(FileProc, ".if_end_%d:\n", save_counter);
    }
    fprintf(FileProc, ";#=========End=IF=========#\n");
}

//done
static void WriteIfCondition(BinaryTree_t* myTree, Node_t* CurrentNode, int curr_counter)
{
    if (!C) 
    {
        fprintf(FileProc, "jmp .if_start_%d", curr_counter);
        return;
    }
    fprintf(FileProc,   ";#========Condition========#\n");
    RecEvaluate(myTree, C);
    fprintf(FileProc,   "\npop rax\n"
                        "cmp rax, 0\n"
                        "jne .if_start_%d\n", curr_counter);
}

//done
static void WriteBreak(BinaryTree_t* myTree, Node_t* CurrentNode, StackInt_t* StackWhileCond)
{
    UNUSED(myTree);
    UNUSED(CurrentNode);
    bool flag = true;
    for (int i = (StackWhileCond->size - 1); i >= 0; i--)
    {
        if (StackWhileCond->data[i] == WHILE_TYPE)
        {
            flag = false;
            fprintf(FileProc,   ";#==========Break=========#\n"
                                "jmp .while_end_%d\n", StackWhileCond->data[i-1]);
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
    UNUSED(myTree);
    UNUSED(CurrentNode);
    bool flag = true; 
    for (int i = (StackWhileCond->size - 1); i >= 0; i--)
    {
        if (StackWhileCond->data[i] == WHILE_TYPE)
        {
            flag = false;
            fprintf(FileProc,   ";#========Continue========#\n"
                                "jmp .while_start_%d\n", StackWhileCond->data[i-1]);
            break;
        }
    }
    if (flag)
    {
        USER_ERROR(0, ERR_BAD_USING_BREAK_CONT, "", return)
    }
}

static int NotInitCheck(BinaryTree_t* myTree, Node_t* CurrentNode)
{
    if (CHECK_TYPE_VAR(CurrentNode->Right))
    {
        fprintf(FileProc,   "\n;#=Undefined=Var=#\n"
                            "call __processing_unassigned_var__\n"
                            ";#=======End=====#\n");
        int index_var = R->Value.Index;
        free(R);
        R = OPR(myTree, EQUAL, VAR(myTree, index_var), NUM(myTree,0));
        R->Parent = C->Right;
        return 1;
    }
    return 0;
}

//CALCULATION FUNCTIONS

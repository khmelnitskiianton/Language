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
static void     WriteDefFunc    (BinaryTree_t* myTree, Node_t* CurrentNode);
static void     WriteCall       (BinaryTree_t* myTree, Node_t* CurrentNode);
static void     WriteVar        (BinaryTree_t* myTree, Node_t* CurrentNode);
static void     WriteWhile      (BinaryTree_t* myTree, Node_t* CurrentNode, StackInt_t* StackWhileCond);
static void     GetArgs         (BinaryTree_t* myTree, Node_t* CurrentNode);
static void     WriteCond       (BinaryTree_t* myTree, Node_t* CurrentNode, StackInt_t* StackWhileCond);
static void     WriteCondCond   (BinaryTree_t* myTree, Node_t* CurrentNode, int curr_counter);
static void     WriteWhileCond  (BinaryTree_t* myTree, Node_t* CurrentNode, int curr_counter);
static void     WriteReturn     (BinaryTree_t* myTree, Node_t* CurrentNode);
static void     WriteBreak      (BinaryTree_t* myTree, Node_t* CurrentNode, StackInt_t* StackWhileCond);
static void     WriteContinue   (BinaryTree_t* myTree, Node_t* CurrentNode, StackInt_t* StackWhileCond);

static char buffer_file[SIZE_OF_OUT_PATH] = {};
static FILE* FileProc = NULL;

static int counter_while = 0;
static int counter_if    = 0;

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
    fprintf(FileProc, ";Start module\nStart:\n\tcall main\nhlt\n\n;Main module\n\n");
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
            case FUNC_DEF: 
                WriteDefFunc(myTree, CurrentNode);
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
static void WriteDefFunc(BinaryTree_t* myTree, Node_t* CurrentNode)
{

}

static void WriteVar(BinaryTree_t* myTree, Node_t* CurrentNode)
{

}

static void WriteCall(BinaryTree_t* myTree, Node_t* CurrentNode)
{
    fprintf(FileProc, ";Push args\n\n");
    GetArgs(myTree, CurrentNode);
    char* name_of_func = myTree->Variables[L->Value.Index].Name;
    fprintf(FileProc, "call %s", name_of_func);
}

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
        //TODO: vars
        return;
    }
    MYASSERT(0, ERR_BAD_END_FUNC, )
}

static void WriteReturn(BinaryTree_t* myTree, Node_t* CurrentNode)
{
    //TODO: ret with var;
    fprintf(FileProc, "ret\n");
}

//done
static void WriteWhile(BinaryTree_t* myTree, Node_t* CurrentNode, StackInt_t* StackWhileCond)
{
    fprintf(FileProc, "\njmp while_check_%d\n", counter_while);
    fprintf(FileProc, "while_start_%d:\n\n", counter_while);
    push_int(StackWhileCond, counter_while);    //save
    push_int(StackWhileCond, WHILE_TYPE);
    counter_while++;
    if (R) {WriteRecTree(myTree, R, StackWhileCond);} //body
    int save_counter  = 0;
    int type_operator = 0;
    pop_int(StackWhileCond, &type_operator);
    pop_int(StackWhileCond, &save_counter);     //pop index
    fprintf(FileProc, "\nwhile_check_%d:\n", save_counter);
    //Condition
    WriteWhileCond(myTree, L, save_counter);
    fprintf(FileProc, "while_end___%d:\n\n", save_counter);
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
    fprintf(FileProc, "push %g\n", R->Value.Number);
    fprintf(FileProc, "push %g\n", L->Value.Number);
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
    fprintf(FileProc, "push %g\n", R->Value.Number);
    fprintf(FileProc, "push %g\n", L->Value.Number);
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
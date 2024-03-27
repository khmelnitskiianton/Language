#include <stdio.h>
#include <string.h>

#include "colors.h"
#include "tree.h"
#include "MyLangConfig.h"
#include "translator.h"
#include "log.h"
#include "output.h"
#include "creator.h"
#include "myassert.h"
#include "verificator.h"
#include "dsl.h"

static EnumOfErrors WriteStart();
static void         WriteRecTree(BinaryTree_t* myTree, Node_t* CurrentNode);
static void         WriteDefFunc(BinaryTree_t* myTree, Node_t* CurrentNode);
static void         WriteCall(BinaryTree_t* myTree, Node_t* CurrentNode);
static void         WriteWhile(BinaryTree_t* myTree, Node_t* CurrentNode);
static void         GetArgs(BinaryTree_t* myTree, Node_t* CurrentNode);
static void         WriteCond(BinaryTree_t* myTree, Node_t* CurrentNode);
static void         WriteCondCond(BinaryTree_t* myTree, Node_t* CurrentNode);
static void         WriteWhileCond(BinaryTree_t* myTree, Node_t* CurrentNode);
static void         WriteReturn(BinaryTree_t* myTree, Node_t* CurrentNode);
static void         WriteBreak(BinaryTree_t* myTree, Node_t* CurrentNode);
static void         WriteContinue(BinaryTree_t* myTree, Node_t* CurrentNode);

static char buffer_file[SIZE_OF_OUT_PATH] = {};
static FILE* FileProc = NULL;

static size_t counter_while = 0;
static size_t counter_if  = 0;


EnumOfErrors TranslateTree(BinaryTree_t* myTree, const char* in_file_path)
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
    WriteRecTree(myTree, myTree->Root);

    return ERR_OK;
}

static EnumOfErrors WriteStart()
{
    fprintf(FileProc, ";Start module\nStart:\n\tcall main\nhlt\n\n;Main module\n\n");
    return ERR_OK;
}

static void WriteRecTree(BinaryTree_t* myTree, Node_t* CurrentNode)
{
    if (!CurrentNode) return;

    if (CurrentNode->Type == OPERATOR)
    {
        if (ArrayOperators[CurrentNode->Value.Index].Class == DIVIDER)
        {
            WriteRecTree(myTree, L);
            WriteRecTree(myTree, R);
        }
        switch (ArrayOperators[CurrentNode->Value.Index].Class)
        {
            case FUNC_DEF: 
                //WriteDefFunc(CurrentNode);
            break;
            case CALL:
                WriteCall(myTree, CurrentNode);
            break;
            case WHILE:
                WriteWhile(myTree, CurrentNode);
            break;
            case IF:
                WriteCond(myTree, CurrentNode);
            break;
            case RETURN:
                WriteReturn(myTree, CurrentNode);
            break;
            case BREAK:
                WriteBreak(myTree, CurrentNode);
            break;
            case CONTINUE:
                WriteContinue(myTree, CurrentNode);
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

static void WriteWhile(BinaryTree_t* myTree, Node_t* CurrentNode)
{
    fprintf(FileProc, "\njmp while_check_%lu\n", counter_while);
    fprintf(FileProc, "while_start_%lu:\n", counter_while);
    //body
    WriteRecTree(myTree, R);
    fprintf(FileProc, "while_check_%lu:\n", counter_while);
    //Condition
    WriteWhileCond(myTree, L);
    fprintf(FileProc, "while_end___%lu:\n", counter_while);
    counter_while++;
}

static void WriteWhileCond(BinaryTree_t* myTree, Node_t* CurrentNode)
{
    if (!C) 
    {
        fprintf(FileProc, "jmp while_start_%lu", counter_while);
        return;
    }
    if (C->Type == NUMBER)
    {
        if (!Compare(C->Value.Number, 0))
        {
            fprintf(FileProc, "jmp while_start_%lu\n", counter_while);
        }
        return;
    }
    fprintf(FileProc, "push %g\n", R->Value.Number);
    fprintf(FileProc, "push %g\n", L->Value.Number);
    switch (ArrayOperators[CurrentNode->Value.Index].Id)
    {
        case 30://>
            fprintf(FileProc, "ja while_start_%lu\n", counter_while);
        break; 
        case 31://<
            fprintf(FileProc, "jb while_start_%lu\n", counter_while);
        break; 
        case 32://>=
            fprintf(FileProc, "jae while_start_%lu\n", counter_while);
        break; 
        case 33://<=
            fprintf(FileProc, "jbe while_start_%lu\n", counter_while);
        break; 
        case 34://!=
            fprintf(FileProc, "jne while_start_%lu\n", counter_while);
        break; 
        case 35://==
            fprintf(FileProc, "je while_start_%lu\n", counter_while);
        break; 
    }   
}

static void WriteCond(BinaryTree_t* myTree, Node_t* CurrentNode)
{
    fprintf(FileProc, "\njmp if_check_%lu\n", counter_if);
    fprintf(FileProc, "if_start_%lu:\n", counter_if);
    //body
    WriteRecTree(myTree, R);
    fprintf(FileProc, "jmp if_end_%lu\n", counter_if);
    fprintf(FileProc, "if_check_%lu:\n", counter_if);
    //Condition
    WriteCondCond(myTree, L);
    fprintf(FileProc, "if_end___%lu:\n", counter_if);
    counter_if++;
}


static void WriteCondCond(BinaryTree_t* myTree, Node_t* CurrentNode)
{
    if (!C) 
    {
        fprintf(FileProc, "jmp while_start_%lu", counter_while);
        return;
    }
    if (C->Type == NUMBER)
    {
        if (!Compare(C->Value.Number, 0))
        {
            fprintf(FileProc, "jmp if_start_%lu\n", counter_while);
        }
        return;
    }
    fprintf(FileProc, "push %g\n", R->Value.Number);
    fprintf(FileProc, "push %g\n", L->Value.Number);
    switch (ArrayOperators[CurrentNode->Value.Index].Id)
    {
        case 30://>
            fprintf(FileProc, "ja if_start_%lu\n", counter_while);
        break; 
        case 31://<
            fprintf(FileProc, "jb if_start_%lu\n", counter_while);
        break; 
        case 32://>=
            fprintf(FileProc, "jae if_start_%lu\n", counter_while);
        break; 
        case 33://<=
            fprintf(FileProc, "jbe if_start_%lu\n", counter_while);
        break; 
        case 34://!=
            fprintf(FileProc, "jne if_start_%lu\n", counter_while);
        break; 
        case 35://==
            fprintf(FileProc, "je if_start_%lu\n", counter_while);
        break; 
    }   
}

static void WriteReturn(BinaryTree_t* myTree, Node_t* CurrentNode)
{
    //TODO: ret with var;
    fprintf(FileProc, "ret\n");
}

static void WriteBreak(BinaryTree_t* myTree, Node_t* CurrentNode)
{
    //TODO:
    fprintf(FileProc, "");
}

static void WriteContinue(BinaryTree_t* myTree, Node_t* CurrentNode)
{
    //TODO:
    fprintf(FileProc, "");
}
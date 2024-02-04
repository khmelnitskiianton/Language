#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>

#include "tree.h"
#include "tokenizer.h"
#include "parser.h"
#include "output.h"
#include "colors.h"
#include "dump.h"
#include "myassert.h"
#include "verificator.h"

/*
    =======================
    |PRINTING TREE TO FILE|
    =======================
*/

//============================================================================================================
static void  PrintRecNode (Node_t* CurrentNode, FILE* filestream);
static void  PrintNodeValue(Node_t* CurrentNode, FILE* filestream);
static void  PrintVars(BinaryTree_t* myTree, FILE* filestream);
static void  CleanCharBuffer(char* buffer, const size_t buffer_size);
static FILE* OpenFile      (const char* file_open, const char* option);
static void  CloseFile     (FILE* file_text);
FILE* FileTree = NULL;
FILE* FileVars = NULL;
//============================================================================================================

EnumOfErrors PrintTree (BinaryTree_t* myTree, const char* out_file_path)
{
    static char buffer_path[SIZE_OF_OUT_PATH] = {};
    snprintf(buffer_path, SIZE_OF_OUT_PATH, "%s/", out_file_path);
    struct stat sb = {};
    char buffer_create[SIZE_OF_OUT_COMMAND] = {};
    if (stat(buffer_path, &sb) || !S_ISDIR(sb.st_mode)) 
    {
        snprintf(buffer_create, SIZE_OF_OUT_COMMAND, "touch %s" FILE_TREE, buffer_path);
        system(buffer_create);
        CleanCharBuffer(buffer_create, SIZE_OF_OUT_COMMAND); 
        snprintf(buffer_create, SIZE_OF_OUT_COMMAND, "touch %s" FILE_VARS, buffer_path);
        system(buffer_create);
        CleanCharBuffer(buffer_create, SIZE_OF_OUT_COMMAND); 
    }
    char buffer_file[SIZE_OF_OUT_COMMAND] = {};
    snprintf(buffer_file, SIZE_OF_OUT_COMMAND, "%s" FILE_TREE, buffer_path);
    FileTree = OpenFile (buffer_file, "w");
    CleanCharBuffer(buffer_create, SIZE_OF_OUT_COMMAND); 
    snprintf(buffer_file, SIZE_OF_OUT_COMMAND, "%s" FILE_VARS, buffer_path);
    FileVars = OpenFile (buffer_file, "w");
    CleanCharBuffer(buffer_create, SIZE_OF_OUT_COMMAND); 
    Verify(myTree);
    
    PrintRecNode(myTree->Root, FileTree);
    PrintVars(myTree, FileVars);

    CloseFile(FileTree);
    CloseFile(FileVars);
    return ERR_OK;
}

static void PrintRecNode (Node_t* CurrentNode, FILE* filestream)
{
    if (!CurrentNode) {fprintf(filestream, " _ "); return;}
    fprintf(filestream, "(");

    PrintNodeValue(CurrentNode, filestream);
    PrintRecNode(CurrentNode->Left, filestream);
    PrintRecNode(CurrentNode->Right, filestream);

    fprintf(filestream, ")");
}

static void PrintNodeValue(Node_t* CurrentNode, FILE* filestream)
{
    fprintf(filestream, "%d", CurrentNode->Type);

    if (CurrentNode->Type == NUMBER)    {fprintf(filestream, " %g ", CurrentNode->Value.Number); return;}
    if (CurrentNode->Type == VARIABLE)  {fprintf(filestream, " %d ", CurrentNode->Value.Index);  return;}
    if (CurrentNode->Type == OPERATOR) 
    {
        fprintf(filestream, " %lu ", ArrayOperators[CurrentNode->Value.Index].Id);
    }
}

static void PrintVars(BinaryTree_t* myTree, FILE* filestream)
{
    for (size_t i = 0; i < SIZE_OF_VARIABLES; i++)
    {
        if (myTree->Variables[i][0] != '\0') fprintf(filestream, "%lu\t%s\n", i, myTree->Variables[i]);
    }
}

static void CleanCharBuffer(char* buffer, const size_t buffer_size)
{
    for (size_t i = 0; i < buffer_size; i++)
    {
        *(buffer + i) = 0;
    }
}

static FILE* OpenFile (const char* file_open, const char* option)
{
    FILE *FileOpen = fopen (file_open, option);
    MYASSERT(FileOpen, ERR_OPEN_FILE, return 0);
    return FileOpen;
}

static void CloseFile (FILE* file_text)
{
	MYASSERT(file_text, ERR_BAD_POINTER_PASSED_IN_FUNC, assert(0));
    int result = fclose(file_text);
	MYASSERT(!result, ERR_CLOSE_FILE, assert(0));
}
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
#include "dsl.h"
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

EnumOfErrors PrintTree (BinaryTree_t* myTree, const char* in_file_path)
{
    //Get file path to create <old_file_path>/<same name>.<extension>
    size_t position = 0;
    char buffer_name_file[SIZE_OF_NAME_FILE] = {};
    while ((in_file_path[position] != '\0') && (in_file_path[position] != '.'))
    {
        buffer_name_file[position] = in_file_path[position];
        position++;
    }
    //In buffer_name_file - name of file
    char buffer_file[SIZE_OF_OUT_PATH] = {};
    snprintf(buffer_file, SIZE_OF_OUT_PATH, "%s" EXT_TREE, buffer_name_file);

    char buffer_create[SIZE_OF_OUT_COMMAND] = {};
    snprintf(buffer_create, SIZE_OF_OUT_COMMAND, "touch %s",buffer_file);
    system(buffer_create);
    CleanCharBuffer(buffer_create, SIZE_OF_OUT_COMMAND); 

    FileTree = OpenFile (buffer_file, "w");
    FileVars = OpenFile (buffer_file, "a");
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

    PrintNodeValue(C, filestream);
    PrintRecNode(L, filestream);
    PrintRecNode(R, filestream);

    fprintf(filestream, ")");
}

static void PrintNodeValue(Node_t* CurrentNode, FILE* filestream)
{
    fprintf(filestream, "%d", CurrentNode->Type);

    if (C->Type == NUMBER)    {fprintf(filestream, " %g ", C->Value.Number); return;}
    if (C->Type == VARIABLE)  {fprintf(filestream, " %d ", C->Value.Index);  return;}
    if (C->Type == OPERATOR) 
    {
        fprintf(filestream, " %lu ", ArrayOperators[C->Value.Index].Id);
    }
}

static void PrintVars(BinaryTree_t* myTree, FILE* filestream)
{
    for (size_t i = 0; i < SIZE_OF_VARIABLES; i++)
    {
        if (myTree->Variables[i][0] != '\0') fprintf(filestream, "\n%lu\t%s", i, myTree->Variables[i]);
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
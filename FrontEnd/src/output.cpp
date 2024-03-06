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

EnumOfErrors PrintTree (BinaryTree_t* myTree, const char* out_file_path, const char* in_file_path)
{
    //Get last '/' in path
    const char* ptr_of_name = NULL; 
    size_t position = 0;
    while (in_file_path[position] != '\0')
    {
        if ((in_file_path[position] == '/') || (in_file_path[position] == '\\'))
        {
            ptr_of_name = in_file_path + position;
        }
        position++;
    }
    ptr_of_name++; //set on first symbol of name

    //Get nam    while (in_file_path[position] != '\0')
    position = 0;
    char buffer_name_file[SIZE_OF_NAME_FILE] = {};
    while ((ptr_of_name[position] != '\0') && (ptr_of_name[position] != '.'))
    {
        buffer_name_file[position] = ptr_of_name[position];
        position++;
    }
    //In buffer_name_file - name of file
    char buffer_file[SIZE_OF_OUT_PATH] = {};
    snprintf(buffer_file, SIZE_OF_OUT_PATH, "%s/%s" EXT_TREE, out_file_path, buffer_name_file);

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
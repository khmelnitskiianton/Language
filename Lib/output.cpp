#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>

#include "tree.h"
#include "output.h"
#include "creator.h"
#include "colors.h"
#include "MyLangConfig.h"
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
static char* CreateFile(const char* in_file_path, const char* extension);
FILE* FileTree = NULL;
FILE* FileVars = NULL;
//============================================================================================================
static char buffer_file[SIZE_OF_OUT_PATH] = {};


EnumOfErrors PrintTree (BinaryTree_t* myTree, const char* in_file_path, const char* extension)
{
    if (!CreateFile(in_file_path, extension)) return ERR_BAD_IN_READ_FILE;

    FileTree = OpenFile (buffer_file, "w");
    FileVars = OpenFile (buffer_file, "a");
    Verify(myTree);
    
    PrintRecNode(myTree->Root, FileTree);
    PrintVars(myTree, FileVars);

    CloseFile(FileTree);
    CloseFile(FileVars);
    return ERR_OK;
}

static char* CreateFile(const char* in_file_path, const char* extension)
{
    //Get file path to create <old_file_path>/<same name>.<extension>
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
    snprintf(buffer_file, SIZE_OF_OUT_PATH, "%s%s", buffer_name_file, extension);

    char buffer_create[SIZE_OF_OUT_COMMAND] = {};
    snprintf(buffer_create, SIZE_OF_OUT_COMMAND, "touch %s",buffer_file);
    system(buffer_create);
    CleanCharBuffer(buffer_create, SIZE_OF_OUT_COMMAND); 
    fprintf(stdout, BLUE "[was created tree file: %s]\n" RESET, buffer_file);
    return buffer_file;
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
        if (myTree->Variables[i].Name[0] != '\0') fprintf(filestream, "\n%lu\t%s", i, myTree->Variables[i].Name);
    }
}
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <time.h>

#include "tree.h"
#include "tokenizer.h"
#include "parser.h"
#include "colors.h"
#include "dump.h"
#include "log.h"
#include "myassert.h"

/*
    ==========================
    |RECURSIVE DESCENT PARSER|
    ==========================
*/

int main(int argc, char *argv[])
{
    //========================================================================================================
    //STARTING

    fprintf(stdout, GREEN "\n<<<FRONTEND BEGIN>>>\n" RESET);
    if (argc == 2)
    {
        printf(GREEN "\n<FILE TO IN/OUT: %s>\n" RESET, argv[1]);
        PrintLogStart(".");
    }
    if (argc == 3)
    {
        printf(GREEN "\n<FILE TO IN/OUT: %s>\n" RESET, argv[1]);
        PrintLogStart(argv[2]);
    }

    //=======================================================================================================
    //INITIALIZATION

    BinaryTree_t myTree = {};
    Tokens_t myTokens = {};
    TreeCtor (&myTree);
    TokensCtor(&myTokens);
    PrintLogTree (&myTree);
    fprintf(stdout, GREEN "Initializing completed!\n\n" RESET);
    
    //==================================================================
    //READ DATA
    
    fprintf(stdout, GREEN "Download Data base...\n" RESET);
    char* code_buffer = CreateDirtyBuffer(argv[1]);
    fprintf(stdout, GREEN "Downloading complete!\n\n" RESET);
    
    //==================================================================
    //CREATE ARRAY OF TOKENS
    
    CreateTokens(&myTokens, code_buffer);
    free(code_buffer); 
    for (size_t i = 0; i < myTokens.Capacity; i++)
    {
        PrintToken(myTokens.Data + i);
    }
    printf("\n");
    for (size_t i = 0; i < SIZE_OF_VARIABLES; i++)
    {
        if (myTokens.Variables[i][0] != 0)
        {
            printf("var [%s]\n", myTokens.Variables[i]);
        }
    }
    printf("\n");
    //==================================================================
    //CREATE RECURSIVE DESCENT

    CopyVars(&myTree, &myTokens);
    myTree.Root = GetMain(&myTokens); 
    PrintLogTree(&myTree);

    //==================================================================
    //DESTRUCTION

    PrintLogFinish();
    TreeDtor(&myTree);
    TokensDtor(&myTokens);
    printf(GREEN "\n<<<FRONTEND END>>>\n\n" RESET);
    return 0;
}
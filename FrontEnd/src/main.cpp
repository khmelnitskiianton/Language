#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "tree.h"
#include "tokenizer.h"
#include "parser.h"
#include "output.h"
#include "colors.h"
#include "dump.h"
#include "log.h"
#include "myassert.h"

/*
    ==========================
    |RECURSIVE DESCENT PARSER|
    ==========================
*/

//=============================================================================

int main(int argc, char *argv[])
{
    //=========================================================================
    //STARTING
    USER_ERROR(argc == 3, ERR_FORGOT_ARGS_IN_START, "",return 0;)

    fprintf(stdout, GREEN "\n<<<FRONTEND BEGIN>>>\n\n" RESET);
    printf(GREEN "<FILE TO READ  CODE:\t%s>\n" RESET, argv[2]);
    printf(GREEN "<FILE TO WRITE LOGS:\t%s>\n" RESET, argv[1]);
    printf(GREEN "<FILE TO WRITE TREE:\t%s>\n" RESET, argv[1]);    

    //=========================================================================
    //CREATE MAIN FOLDER WITH RESULTS
    
    struct stat sb = {};
    char buffer_create_folder[SIZE_OF_COMMAND] = {};
    if (stat(argv[1], &sb) || !S_ISDIR(sb.st_mode)) 
    { 
        snprintf(buffer_create_folder, SIZE_OF_COMMAND, "mkdir %s", argv[1]);
        system(buffer_create_folder);
    }

    //=========================================================================
    //INITIALIZATION

    BinaryTree_t myTree   = {};
    Tokens_t     myTokens = {};
    TreeCtor(&myTree);
    TokensCtor(&myTokens);
    PrintLogStart(argv[1], argv[2]);
    PrintLogTree(&myTree);
    fprintf(stdout, GREEN "\nInitializing complete!\n\n" RESET);
    
    //=========================================================================
    //READ DATA
    
    fprintf(stdout, GREEN "Read file...\n" RESET);
    char* code_buffer = CreateDirtyBuffer(argv[2]);
    fprintf(stdout, GREEN "Reading complete!\n\n" RESET);
    
    //=========================================================================
    //CREATE ARRAY OF TOKENS
    
    CreateTokens(&myTokens, code_buffer);
    free(code_buffer); 

    // for (size_t i = 0; i < myTokens.Capacity; i++)
    // {
        // PrintToken(myTokens.Data + i);
    // }
    // printf("\n");
    // for (size_t i = 0; i < SIZE_OF_VARIABLES; i++)
    // {
        // if (myTokens.Variables[i][0] != 0)
        // {
            // printf("var [%s]\n", myTokens.Variables[i]);
        // }
    // }
    // printf("\n");

    //=========================================================================
    //CREATE RECURSIVE DESCENT

    CopyVars(&myTree, &myTokens);
    myTree.Root = GetMain(&myTokens, &myTree); 
    PrintLogTree(&myTree);

    //=========================================================================
    //PRINT TREE

    PrintTree(&myTree, argv[1], argv[2]);

    //=========================================================================
    //DESTRUCTION

    PrintLogFinish();
    TreeDtor(&myTree);
    TokensDtor(&myTokens);
    fprintf(stdout, GREEN "\nDestruction complete!\n" RESET);

    //=========================================================================
    //END
    printf(GREEN "\n<<<FRONTEND END>>>\n\n" RESET);
    
    return 0;
}
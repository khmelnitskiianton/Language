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
    USER_ERROR(argc == 3, ERR_FORGOT_ARGS_IN_START, "\0",return 0;)
    fprintf(stdout, GREEN "\n<<<FRONTEND BEGIN>>>\n" RESET);
    printf(GREEN "<FILE TO READ  CODE: %s>\n" RESET, argv[2]);
    printf(GREEN "<FILE TO WRITE LOGS: %s>\n" RESET, argv[1]);
    printf(GREEN "<FILE TO WRITE TREE: %s>\n" RESET, argv[1]);
    //=========================================================================
    //CREATE MAIN FOLDER WITH TEMPORARY FILES
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
    fprintf(stdout, GREEN "#Initializing complete!\n" RESET);
    //=========================================================================
    //READ DATA
    char* code_buffer = CreateDirtyBuffer(argv[2]);
    fprintf(stdout, GREEN "#Reading complete!\n" RESET);
    //=========================================================================
    //CREATE ARRAY OF TOKENS
    CreateTokens(&myTokens, code_buffer);
    free(code_buffer); 
    //=========================================================================
    //CREATE RECURSIVE DESCENT
    CopyVars(&myTree, &myTokens);
    myTree.Root = GetMain(&myTokens, &myTree); 
    PrintLogTree(&myTree);
    //=========================================================================
    //PRINT TREE
    PrintTree(&myTree, argv[2]);
    //=========================================================================
    //DESTRUCTION
    PrintLogFinish();
    TreeDtor(&myTree);
    TokensDtor(&myTokens);
    fprintf(stdout, GREEN "#Destruction complete!\n" RESET);
    //=========================================================================
    //END
    printf(GREEN "<<<FRONTEND END>>>\n\n" RESET);
    return 0;
}
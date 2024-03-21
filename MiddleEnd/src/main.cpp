#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "tree.h"
#include "creator.h"
#include "optimizier.h"
#include "colors.h"
#include "MyLangConfig.h"
#include "output.h"
#include "log.h"
#include "myassert.h"
#include "verificator.h"

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
    fprintf(stdout, GREEN "\n<<<middleend begin>>>\n" RESET);
    printf(GREEN "<file to read  code: %s>\n" RESET, argv[2]);
    printf(GREEN "<file to write logs: %s>\n" RESET, argv[1]);
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
    BinaryTree_t myTree = {};
    TreeCtor(&myTree);
    if (PrintLogStart(argv[1], argv[2]) == ERR_NO_FILE_TO_OPEN) 
    {
        USER_ERROR(0, ERR_NO_FILE_TO_OPEN, "", exit(0))
    }
    PrintLogTree(&myTree);
    fprintf(stdout, GREEN "#initializing complete!\n" RESET);
    //=========================================================================
    //UPLOADING & CREATING TREE
    EnumOfErrors upload_status = ERR_OK;
    upload_status = UploadTree(&myTree, argv[2]);
    if (upload_status != ERR_OK)
    {
        goto destruction_label;
    }
    PrintLogTree(&myTree);
        fprintf(stdout, GREEN "#reading complete!\n" RESET);
    //=========================================================================
    //OPTIMIZATION
    TreeOptimize(&myTree);
    PrintLogTree(&myTree);
    //=========================================================================
    //PRINT TREE
    PrintTree(&myTree, argv[2]);
    //=========================================================================
    //DESTRUCTION
destruction_label:
    PrintLogFinish();
    TreeDtor(&myTree);
    fprintf(stdout, GREEN "#destruction complete!\n" RESET);
    //=========================================================================
    //END
    printf(GREEN "<<<middleend end>>>\n\n" RESET);
    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <time.h>

#include "colors.h"
#include "dump.h"
#include "tree.h"
#include "tree_functions.h"
#include "log.h"
#include "myassert.h"
#include "tokenizer.h"

int main(int argc, char *argv[])
{
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

    BinaryTree_t myTree = {};
    Tokens_t myTokens = {};
    TreeCtor (&myTree);
    TokensCtor(&myTokens);
    PrintLogTree (&myTree);
    fprintf(stdout, GREEN "Initializing completed!\n\n" RESET);
    //==================================================================
    //Buffer
    fprintf(stdout, GREEN "Download Data base...\n" RESET);
    char* dirty_buffer = CreateDirtyBuffer(argv[1]);
    fprintf(stdout, GREEN "Downloading complete!\n\n" RESET);
    //==================================================================
    //Tokens array
    CreateTokens(&myTokens, dirty_buffer);
    free(dirty_buffer);
    for (size_t i = 0; i < myTokens.Capacity; i++)
    {
        Token_t* CurrToken = (myTokens.Data + i);
        if (CurrToken->Type == NUMBER)   printf("[num   %lg]\n", CurrToken->Value.Number);
        if (CurrToken->Type == OPERATOR) printf("[oper  %s]\n",  ArrayOperators[CurrToken->Value.Index].Name);
        if (CurrToken->Type == BRACKET)  printf("[brack %s]\n",  ArrayBrackets[CurrToken->Value.Index]);
        if (CurrToken->Type == VARIABLE) printf("[var   %s]\n",  myTokens.Variables[CurrToken->Value.Index]);
        if (CurrToken->Type == END)      printf("[end     ]\n");
    }
    //==================================================================
    //Recursion(Creating Tree)

    //==================================================================
    PrintLogFinish();

    TreeDtor(&myTree);
    TokensDtor(&myTokens);
    printf(GREEN "\n<<<FRONTEND END>>>\n\n" RESET);
    return 0;
}
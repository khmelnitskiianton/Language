#ifndef PARSER_H
#define PARSER_H

Node_t* GetMain (Tokens_t* myTokens, BinaryTree_t* myTree);
EnumOfErrors CopyVars(BinaryTree_t* myTree, Tokens_t* myTokens);

#include "MyLangConfig.h"

#endif
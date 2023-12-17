#ifndef TREE_FUNCTIONS_H
#define TREE_FUNCTIONS_H

#include "errors_enum.h"

void         TreeCtor   (BinaryTree_t* myTree);
EnumOfErrors TreeDtor   (BinaryTree_t* myTree);
void         InitNode   (Node_t* NewNode);
Node_t*      CreateNode (BinaryTree_t* myTree);

#endif
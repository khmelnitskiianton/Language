#include <stdio.h>
#include <stdlib.h>

#include "tree.h"
#include "verificator.h"

const bool FindLoop = 1;
const bool NoLoop = 0;
bool CheckLoop(BinaryTree_t* myTree);
bool LookNode(Node_t* CurrentNode, BinaryTree_t* myTree, size_t amount_of_nodes);

EnumOfErrors _Verify(BinaryTree_t* myTree)
{
    if (!myTree)            return ERR_BAD_POINTER_TREE;     
    if (!(myTree->Root))    return ERR_ROOT_NULL;
    
    if (CheckLoop(myTree))  return ERR_LOOP_IN_TREE;

    return ERR_OK;
}

bool CheckLoop(BinaryTree_t* myTree)
{
    size_t amount_of_nodes = 0;
    return LookNode(myTree->Root, myTree, amount_of_nodes);
}

bool LookNode(Node_t* CurrentNode, BinaryTree_t* myTree, size_t amount_of_nodes)
{
    amount_of_nodes++;

    if (amount_of_nodes > myTree->Size)
    {
        return FindLoop;
    }

    if (CurrentNode->Left) 
    {
        return LookNode(CurrentNode->Left, myTree, amount_of_nodes);
    }

    if (CurrentNode->Right) 
    {
        return LookNode(CurrentNode->Right, myTree, amount_of_nodes);
    }

    return NoLoop;
}
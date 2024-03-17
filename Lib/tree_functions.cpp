#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "tree.h" 
#include "myassert.h"
#include "verificator.h"

void TreeCtor (BinaryTree_t* myTree)
{
    MYASSERT(myTree, ERR_BAD_POINTER_TREE, return)

    myTree->Root = NULL;
    myTree->Size = 0;
    for (size_t i = 0; i < SIZE_OF_VARIABLES; i++)
    {
        myTree->Variables[i].Name[0] = '\0';
        myTree->Variables[i].Number = 0;
    }
}

EnumOfErrors TreeDtor (BinaryTree_t* myTree)
{
    MYASSERT(myTree, ERR_BAD_POINTER_TREE, return ERR_BAD_POINTER_TREE)

    if (myTree->Root == NULL)
    {
        return ERR_OK;
    }

    //Verify(myTree);

    RecFree (myTree->Root); 
    return ERR_OK;
}

void RecFree (Node_t* CurrentNode)
{
    if (!CurrentNode) return;
    
    if (CurrentNode->Left)
    {
        RecFree (CurrentNode->Left);
    }
    if (CurrentNode->Right)
    {
        RecFree (CurrentNode->Right);
    }
    free(CurrentNode);
}

void InitNode(Node_t* NewNode)
{
    MYASSERT(NewNode, ERR_BAD_POINTER_NODE, return)

    NewNode->Value.Number  = NAN;
    NewNode->Type          = INIT;
    NewNode->Right         = NULL;
    NewNode->Left          = NULL;
    NewNode->Parent        = NULL;
}

Node_t* CreateNode (BinaryTree_t* myTree)
{
    Node_t* NewNode = (Node_t*) calloc (1, sizeof (Node_t));
    MYASSERT(NewNode, ERR_BAD_CALLOC, return NULL)
    InitNode(NewNode);
    myTree->Size++;
    return NewNode;
}


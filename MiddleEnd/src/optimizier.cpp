#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "colors.h"
#include "tree.h"
#include "optimizier.h"
#include "calculation.h"
#include "MyLangConfig.h"
#include "log.h"
#include "myassert.h"
#include "verificator.h"
#include "dsl.h"

static bool         RecOptimizeConst(Node_t* CurrentNode, BinaryTree_t* myTree);
static EnumOfErrors RecOptimizeNeutral(Node_t* CurrentNode, BinaryTree_t* myTree);
static EnumOfErrors DeleteNeutralNode(Node_t* NeutralNode, Node_t* BranchNode, BinaryTree_t* myTree);
static EnumOfErrors DeleteNeutralBranch(Node_t* CurrentNode, double NewValue);
static EnumOfErrors RecOptimizeCondLoop(Node_t* CurrentNode, BinaryTree_t* myTree);

EnumOfErrors TreeOptimize(BinaryTree_t* myTree)
{
    TREE_CHANGED;
    while (myTree->ChangeOptimize)
    {
        myTree->ChangeOptimize = 0;
        RecOptimizeConst   (myTree->Root, myTree);
        RecOptimizeNeutral (myTree->Root, myTree);
        RecOptimizeCondLoop(myTree->Root, myTree);
        PrintLogTree(myTree);
    }
    return ERR_OK;
}

//1. Const killing done
/* 
    0*... ...*0 1*... ...*1
    0+... ...+0
    0-... ...-0
    .../1
    ...^0 0^... ...^1 1^...
    &&
    ||
*/

static EnumOfErrors RecOptimizeNeutral(Node_t* CurrentNode, BinaryTree_t* myTree)
{
    if (!C) return ERR_OK;
    if (C->Type == OPERATOR)
    {
        int index_arith_oper = FindArithOper(C);   //index in calc_operators arith 
        if (index_arith_oper == -1) goto skip_switch; 
        PrintLogTree(myTree);
        switch (CalcOperators[index_arith_oper].Id)
        {
            case 40: //+ 
                if (ISNUM(L, 0)) {DeleteNeutralNode(L, R, myTree);  TREE_CHANGED; return ERR_OK;}
                if (ISNUM(R, 0)) {DeleteNeutralNode(R, L, myTree);  TREE_CHANGED; return ERR_OK;}            
            break;
            case 41: //-
                if (ISNUM(R, 0)) {DeleteNeutralNode(R, L, myTree);  TREE_CHANGED; return ERR_OK;}
            break;
            case 53: ///
                if (ISNUM(R, 1)) {DeleteNeutralNode(R, L, myTree);  TREE_CHANGED; return ERR_OK;}
            break;
            case 60: //^
                if (ISNUM(R, 1)) {DeleteNeutralNode(R, L, myTree);  TREE_CHANGED; return ERR_OK;}
                if (ISNUM(L, 1)) {DeleteNeutralBranch(C, 1);        TREE_CHANGED; return ERR_OK;}
                if (ISNUM(R, 0)) {DeleteNeutralBranch(C, 1);        TREE_CHANGED; return ERR_OK;}
                if (ISNUM(L, 0)) {DeleteNeutralBranch(C, 0);        TREE_CHANGED; return ERR_OK;}
            break;
            case 52: //*
                if (ISNUM(L, 1)) {DeleteNeutralNode(L, R, myTree);  TREE_CHANGED; return ERR_OK;}
                if (ISNUM(R, 1)) {DeleteNeutralNode(R, L, myTree);  TREE_CHANGED; return ERR_OK;}
                if (ISNUM(L, 0)) {DeleteNeutralBranch(C, 0);        TREE_CHANGED; return ERR_OK;}
                if (ISNUM(R, 0)) {DeleteNeutralBranch(C, 0);        TREE_CHANGED; return ERR_OK;}
            break;
            case 50: //&&
                if (ISNUM(L, 1)) {DeleteNeutralNode(L, R, myTree);  TREE_CHANGED; return ERR_OK;}
                if (ISNUM(R, 1)) {DeleteNeutralNode(R, L, myTree);  TREE_CHANGED; return ERR_OK;}
                if (ISNUM(L, 0)) {DeleteNeutralBranch(C, 0);        TREE_CHANGED; return ERR_OK;}
                if (ISNUM(R, 0)) {DeleteNeutralBranch(C, 0);        TREE_CHANGED; return ERR_OK;}
            break;
            case 42: //+ 
                if (ISNUM(L, 0)) {DeleteNeutralNode(L, R, myTree);  TREE_CHANGED; return ERR_OK;}
                if (ISNUM(R, 0)) {DeleteNeutralNode(R, L, myTree);  TREE_CHANGED; return ERR_OK;}            
            break;
            default: break;
        }
    }
skip_switch:
    RecOptimizeNeutral(L, myTree);
    RecOptimizeNeutral(R, myTree);
    return ERR_OK;
}

static EnumOfErrors DeleteNeutralNode(Node_t* NeutralNode, Node_t* BranchNode, BinaryTree_t* myTree)
{
    free(NeutralNode);
    Node_t* PreviousNode = BranchNode->Parent;
    //началась пересвязка
    if (!(PreviousNode->Parent)) //корень!
    {
        myTree->Root       = BranchNode;
        BranchNode->Parent = NULL;
        free(PreviousNode);
        return ERR_OK;
    }
    Node_t* NewPreviousNode = PreviousNode->Parent;
    if (NewPreviousNode->Left == PreviousNode)  NewPreviousNode->Left = BranchNode;
    else                                        NewPreviousNode->Right = BranchNode;
    BranchNode->Parent = NewPreviousNode;
    free(PreviousNode);
    return ERR_OK;
}


static EnumOfErrors DeleteNeutralBranch(Node_t* CurrentNode, double NewValue)
{
    //Clean
    RecFree(L);
    RecFree(R);
    //Change nodes
    L = NULL;
    R = NULL;
    C->Type = NUMBER;
    C->Value.Number = NewValue;
    return ERR_OK;
}

//2. Delete neutral elements
static bool RecOptimizeConst(Node_t* CurrentNode, BinaryTree_t* myTree)
{
    if (!C)                   return 1;
    if (C->Type == NUMBER)    return 1;
    if (C->Type == VARIABLE)  return 0;

    //if call standart function   
    if ((C->Type == OPERATOR)&&(ArrayOperators[C->Value.Index].Class == CALL))
    {
        //check for name
        int index_func_oper = FindFuncOper(L, myTree);
        bool right_expr     = RecOptimizeConst(R, myTree);
        if (right_expr && index_func_oper)
        {
            C->Type = NUMBER;
            if (CalcOperators[index_func_oper].TypeOperator) C->Value.Number = CalcOperators[index_func_oper].Operation(L->Value.Number, R->Value.Number);
            else C->Value.Number = CalcOperators[index_func_oper].Operation(R->Value.Number, NAN);
            free(L);
            free(R);
            L = NULL;
            R = NULL;
            TREE_CHANGED;
            return 1;   
        }
        return 0;
    }
    //then its expression not func
    bool left  = RecOptimizeConst(L, myTree);
    bool right = RecOptimizeConst(R, myTree);

    int index_arith_oper = FindArithOper(C);   //index in calc_operators arith 
    //if arithmetic operator
    if (left && right && (index_arith_oper != -1))
    {   
        C->Type = NUMBER;
        if (CalcOperators[index_arith_oper].TypeOperator) C->Value.Number = CalcOperators[index_arith_oper].Operation(L->Value.Number, R->Value.Number);
        else C->Value.Number = CalcOperators[index_arith_oper].Operation(L->Value.Number, NAN);
        free(L);
        free(R);
        L = NULL;
        R = NULL;
        TREE_CHANGED;
        return 1;   
    }

    return 0;
}

static EnumOfErrors RecOptimizeCondLoop(Node_t* CurrentNode, BinaryTree_t* myTree)
{
    if (!C) return ERR_OK;
    if ((C->Type == OPERATOR)&&(ArrayOperators[C->Value.Index].Class == IF))
    {
        bool cond_number = (L->Type == NUMBER);
        bool else_node = 0;
        if (R)
        {
            else_node = ((R->Type == OPERATOR) && (ArrayOperators[R->Value.Index].Class == ELSE));
        }
        if (cond_number && (!else_node)) 
        {
            if (Compare(L->Value.Number, 0))    // if (0) {...}
            {
                if ((P->Left)  == C) (P->Left)  = NULL;
                if ((P->Right) == C) (P->Right) = NULL;
                RecFree(C);
                TREE_CHANGED; 
                return ERR_OK;
            }
            if (!Compare(L->Value.Number, 0))    // if (number not zero) {...}
            {
                if (!(C->Right))
                {
                    if (((P->Left)  == C)) {P->Left  = NULL;}
                    if (((P->Right) == C)) {P->Right = NULL;}   
                }
                if (((P->Left)  == C)) {(P->Left)  = R; (R->Parent) = P;}
                if (((P->Right) == C)) {(P->Right) = R; (R->Parent) = P;}
                free(L);
                free(C);
                TREE_CHANGED;
                return ERR_OK;
            }
        }
        if (cond_number && else_node) 
        {
            if (Compare(L->Value.Number, 0))    // if (0) {...}
            {
                if ((P->Left)  == C) {(P->Left)  = R->Right; R->Right->Parent = P;}
                if ((P->Right) == C) {(P->Right) = R->Right; R->Right->Parent = P;}
                RecFree(R->Left);
                free(R);
                free(L);
                free(C);
                TREE_CHANGED; 
                return ERR_OK;
            }
            if (!Compare(L->Value.Number, 0))    // if (1) {...}
            {
                if ((P->Left)  == C) {(P->Left)  = R->Left; R->Left->Parent = P;}
                if ((P->Right) == C) {(P->Right) = R->Left; R->Left->Parent = P;}
                RecFree(R->Right);
                free(R);
                free(L);
                free(C);
                TREE_CHANGED; 
                return ERR_OK;
            }
        }
    }
    if ((C->Type == OPERATOR)&&(ArrayOperators[C->Value.Index].Class == WHILE))
    {
        if (Compare(L->Value.Number, 0))    // if (0) {...}
        {
            if ((P->Left)  == C) (P->Left)  = NULL;
            if ((P->Right) == C) (P->Right) = NULL;
            RecFree(C);
            TREE_CHANGED; 
            return ERR_OK;
        }
    }
    RecOptimizeCondLoop(L, myTree);
    RecOptimizeCondLoop(R, myTree);
    return ERR_OK;
}
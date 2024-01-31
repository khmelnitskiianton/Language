#ifndef TREE_H
#define TREE_H

//==============================================================================================================
//ABOUT ELEMENT

#include <math.h>
#include <limits.h>

typedef double Elem_t;

#define SPECIFIER_SCANF "%lf"
#define SPECIFIER_INIT " %.3lf "
#define SPECIFIER_NUMBER " %.3lf "
#define SPECIFIER_OPERATOR_STR " %s "
#define SPECIFIER_VARIABLE_STR " %s "
#define SPECIFIER_VARIABLE_NUM " %.3lf "

const Elem_t POISON_ELEMENT = NAN;
const double EPSILONE = 1e-5;

const size_t SIZE_OF_VARIABLES = 40;   //amount of variables
const size_t SIZE_OF_VAR = 100;        //max length of variable

//==============================================================================================================
//ABOUT TREE

enum EnumOfType {
    INIT     = 0,
    NUMBER   = 1,
    OPERATOR = 2,
    VARIABLE = 3,
    END      = 4,
};

typedef struct NodeValue {
    Elem_t Number;
    int    Index;
} NodeValue_t;

struct Node {
    NodeValue_t  Value;
    EnumOfType   Type;
    struct Node* Right;
    struct Node* Left;
    struct Node* Parent;
};

typedef struct Node Node_t;

typedef struct BinaryTree {
    Node_t*  Root;
    size_t   Size;
    bool     ChangeOptimize;
    char     Variables[SIZE_OF_VARIABLES][SIZE_OF_VAR];
} BinaryTree_t;

//==============================================================================================================
//ABOUT FUNCTIONS

#include "errors_enum.h"

void            TreeCtor    (BinaryTree_t* myTree);
EnumOfErrors    TreeDtor    (BinaryTree_t* myTree);
void            InitNode    (Node_t* NewNode);
Node_t*         CreateNode  (BinaryTree_t* myTree);
void            RecFree     (Node_t* CurrentNode);

#endif
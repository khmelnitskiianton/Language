#ifndef TREE_H
#define TREE_H

#include "element.h"

enum EnumOfType {
    INIT     = 0,
    NUMBER   = 1,
    OPERATOR = 2,
    VARIABLE = 3,
    BRACKET  = 4,
    END      = 5,
};

// enum EnumOperIndex {
    
// }

enum EnumOperType {
    UNKNOWN = 0,
    SYMBOL  = 1,
    LETTER  = 2,
};

typedef struct Operator {
    const char*     Name;
    EnumOperType    Type;    
} Operator_t;

constexpr Operator_t ArrayOperators[] = {
    {"!=",      SYMBOL},
    {"==",      SYMBOL},
    {">",       SYMBOL},
    {"<",       SYMBOL},
    {"<=",      SYMBOL},
    {">=",      SYMBOL},
    {"/",       SYMBOL},
    {"*",       SYMBOL},
    {"+",       SYMBOL},
    {"-",       SYMBOL},
    {"%",       SYMBOL},
    {"=",       SYMBOL},
    {";",       SYMBOL},
    {"if",      LETTER},
    {"while",   LETTER},
    {"int",     LETTER},
    {"char",    LETTER},
    {"double",  LETTER},
    {"return",  LETTER},
};

constexpr const char* ArrayBrackets[]  = {"(", ")", "{", "}", "[", "]"};

const size_t SIZE_OF_OPERATORS = sizeof(ArrayOperators)/sizeof(ArrayOperators[0]);
const size_t SIZE_OF_BRACKETS  = sizeof(ArrayBrackets)/sizeof(ArrayBrackets[0]);

const size_t SIZE_OF_VARIABLES = 30;   //макс. кол-во переменных
const size_t SIZE_OF_VAR = 100;        //макс. размер одной переменной, можно сделать динамикой

const char COMMENT_SYMBOL = '$';

//================================
//Токены

typedef struct TokenValue {
    Elem_t Number;
    int    Index;
} TokenValue_t;

typedef struct Token {
    EnumOfType Type;
    TokenValue Value;
}Token_t;

typedef struct Tokens {
    Token_t* Data;
    size_t   Capacity;
    int      Size;
    char     Variables[SIZE_OF_VARIABLES][SIZE_OF_VAR];
} Tokens_t;

//=================================
//Дерево

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

#endif
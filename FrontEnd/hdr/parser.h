#ifndef PARSER_H
#define PARSER_H

Node_t* GetMain (Tokens_t* myTokens);
size_t  CopyVars(BinaryTree_t* myTree, Tokens_t* myTokens);

//==============================================================================================================
//ABOUT OPERATORS AND LANGUAGE

enum EnumOperType {
    UNKNOWN = 0,
    SYMBOL  = 1,
    LETTER  = 2,
};

enum EnumOperNum {
    SOLO = 0,
    MULT = 1,
};

enum EnumOperClass{
    ZERO,  
    FIRST, 
    SECOND,
    THIRD,
    FOURTH,
    EQUAL,
    DIVIDER,
    DIVIDER_ARG,
    OP_BR_ONE,
    CL_BR_ONE,
    OP_BR_TWO,
    CL_BR_TWO,
    OP_BR_THREE,
    CL_BR_THREE,
    VAR,
    VOID,
    IF,
    ELSE,
    WHILE,
    BREAK,
    CONTINUE,
    RETURN, 
    FUNC_DEF,
    FUNC_DEF_HELP,
    CALL,
};

typedef struct Operator {
    const char*     Name;
    EnumOperType    Type;  
    EnumOperNum     Num; //CAN SYMBOL BE REPEATED MANY TIMES
    EnumOperClass   Class; //FOR PARSER CATEGORY
} Operator_t;

constexpr Operator_t ArrayOperators[] = {
    {"!=",              SYMBOL,         MULT,       ZERO},
    {"==",              SYMBOL,         MULT,       ZERO},
    {">",               SYMBOL,         MULT,       ZERO},
    {"<",               SYMBOL,         MULT,       ZERO},
    {"<=",              SYMBOL,         MULT,       ZERO},
    {">=",              SYMBOL,         MULT,       ZERO},

    {"++",              SYMBOL,         MULT,       FOURTH},
    {"--",              SYMBOL,         MULT,       FOURTH},
    {"!",               SYMBOL,         MULT,       FOURTH},    

    {"||",              SYMBOL,         MULT,       FIRST},
    {"+",               SYMBOL,         MULT,       FIRST},
    {"-",               SYMBOL,         MULT,       FIRST},

    {"&&",              SYMBOL,         MULT,       SECOND},
    {"/",               SYMBOL,         MULT,       SECOND},
    {"*",               SYMBOL,         MULT,       SECOND},
    {"%",               SYMBOL,         MULT,       SECOND},

    {"^",               SYMBOL,         MULT,       THIRD},

    {"=",               SYMBOL,         MULT,       EQUAL},
    {";",               SYMBOL,         MULT,       DIVIDER},
    {",",               SYMBOL,         MULT,       DIVIDER_ARG},

    {"(",               SYMBOL,         MULT,       OP_BR_ONE},
    {")",               SYMBOL,         MULT,       CL_BR_ONE},
    {"{",               SYMBOL,         MULT,       OP_BR_TWO},
    {"}",               SYMBOL,         MULT,       CL_BR_TWO},
    {"[",               SYMBOL,         MULT,       OP_BR_THREE},
    {"]",               SYMBOL,         MULT,       CL_BR_THREE},

    {"var",             LETTER,         SOLO,       VAR},
    {"void",            LETTER,         SOLO,       VOID},
    {"if",              LETTER,         SOLO,       IF},
    {"else",            LETTER,         SOLO,       ELSE},
    {"while",           LETTER,         SOLO,       WHILE},
    {"break",           LETTER,         SOLO,       BREAK},
    {"continue",        LETTER,         SOLO,       CONTINUE},
    {"return",          LETTER,         SOLO,       RETURN},
    {"func_def",        LETTER,         SOLO,       FUNC_DEF},
    {"func_def_help",   LETTER,         SOLO,       FUNC_DEF_HELP},
    {"call",            LETTER,         SOLO,       CALL},
};
const size_t SIZE_OF_OPERATORS = sizeof(ArrayOperators)/sizeof(ArrayOperators[0]);

const char COMMENT_SYMBOL_UNO = '#';
const char COMMENT_SYMBOL_DUO = '$';

#endif
#ifndef PARSER_H
#define PARSER_H

Node_t* GetMain (Tokens_t* myTokens, BinaryTree_t* myTree);
EnumOfErrors CopyVars(BinaryTree_t* myTree, Tokens_t* myTokens);

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
    size_t          Id;//FOR STANDART OF TREE
} Operator_t;

constexpr Operator_t ArrayOperators[] = {
    {">",               SYMBOL,         MULT,       ZERO,               52},
    {"<",               SYMBOL,         MULT,       ZERO,               53},
    {">=",              SYMBOL,         MULT,       ZERO,               54},
    {"<=",              SYMBOL,         MULT,       ZERO,               55},
    {"!=",              SYMBOL,         MULT,       ZERO,               57},
    {"==",              SYMBOL,         MULT,       ZERO,               56},

    {"++",              SYMBOL,         MULT,       FOURTH,             106},
    {"--",              SYMBOL,         MULT,       FOURTH,             108},
    {"!",               SYMBOL,         MULT,       FOURTH,             205},    

    {"+",               SYMBOL,         MULT,       FIRST,              101},
    {"-",               SYMBOL,         MULT,       FIRST,              102},
    {"||",              SYMBOL,         MULT,       FIRST,              204},

    {"&&",              SYMBOL,         MULT,       SECOND,             203},
    {"%",               SYMBOL,         MULT,       SECOND,             102},
    {"*",               SYMBOL,         MULT,       SECOND,             103},
    {"/",               SYMBOL,         MULT,       SECOND,             104},

    {"^",               SYMBOL,         MULT,       THIRD,              109},

    {"=",               SYMBOL,         MULT,       EQUAL,              51},
    {";",               SYMBOL,         MULT,       DIVIDER,            1},
    {",",               SYMBOL,         MULT,       DIVIDER_ARG,        5},

    {"(",               SYMBOL,         MULT,       OP_BR_ONE,          0},
    {")",               SYMBOL,         MULT,       CL_BR_ONE,          0},
    {"{",               SYMBOL,         MULT,       OP_BR_TWO,          0},
    {"}",               SYMBOL,         MULT,       CL_BR_TWO,          0},
    {"[",               SYMBOL,         MULT,       OP_BR_THREE,        0},
    {"]",               SYMBOL,         MULT,       CL_BR_THREE,        0},

    {"var",             LETTER,         SOLO,       VAR,                207},
    {"void",            LETTER,         SOLO,       VOID,               208},
    {"if",              LETTER,         SOLO,       IF,                 201},
    {"while",           LETTER,         SOLO,       WHILE,              202},
    {"else",            LETTER,         SOLO,       ELSE,               206},
    {"return",          LETTER,         SOLO,       RETURN,             402},
    {"break",           LETTER,         SOLO,       BREAK,              406},
    {"continue",        LETTER,         SOLO,       CONTINUE,           407},

    {"func_def",        LETTER,         SOLO,       FUNC_DEF,           3},
    {"func_def_help",   LETTER,         SOLO,       FUNC_DEF_HELP,      4},
    {"call",            LETTER,         SOLO,       CALL,               401},
};
const size_t SIZE_OF_OPERATORS = sizeof(ArrayOperators)/sizeof(ArrayOperators[0]);

const char COMMENT_SYMBOL_UNO = '#';
const char COMMENT_SYMBOL_DUO = '$';

#endif
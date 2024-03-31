#ifndef MY_LANG_CONFIG_H
#define MY_LANG_CONFIG_H

//Config of my language 

const char COMMENT_SYMBOL_UNO = '#';
const char COMMENT_SYMBOL_DUO = '$';

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
    EnumOperNum     Comp;
    EnumOperClass   Class; //FOR PARSER CATEGORY
    size_t          Id;//FOR STANDART OF TREE
} Operator_t;
 
constexpr Operator_t ArrayOperators[] = {
    {"func_def",        LETTER,         MULT,       FUNC_DEF,           10},
    {"func_def_help",   LETTER,         MULT,       FUNC_DEF_HELP,      11},
    {"call",            LETTER,         MULT,       CALL,               12},
    {"return",          LETTER,         MULT,       RETURN,             13},

    {";",               SYMBOL,         SOLO,       DIVIDER,            20},
    {",",               SYMBOL,         MULT,       DIVIDER_ARG,        21},
    {"=",               SYMBOL,         MULT,       EQUAL,              22},   

    {"var",             LETTER,         MULT,       VAR,                100},
    {"void",            LETTER,         MULT,       VOID,               101},
    {"if",              LETTER,         MULT,       IF,                 102},
    {"while",           LETTER,         MULT,       WHILE,              103},
    {"else",            LETTER,         MULT,       ELSE,               104},
    {"break",           LETTER,         MULT,       BREAK,              105},
    {"continue",        LETTER,         MULT,       CONTINUE,           106},

    {">",               SYMBOL,         MULT,       ZERO,               30},
    {"<",               SYMBOL,         MULT,       ZERO,               31},
    {">=",              SYMBOL,         MULT,       ZERO,               32},
    {"<=",              SYMBOL,         MULT,       ZERO,               33},
    {"!=",              SYMBOL,         MULT,       ZERO,               34},
    {"==",              SYMBOL,         SOLO,       ZERO,               35},

    {"+",               SYMBOL,         MULT,       FIRST,              40},
    {"-",               SYMBOL,         MULT,       FIRST,              41},
    {"||",              SYMBOL,         MULT,       FIRST,              42},

    {"&&",              SYMBOL,         MULT,       SECOND,             50},
    {"%",               SYMBOL,         MULT,       SECOND,             51},
    {"*",               SYMBOL,         MULT,       SECOND,             52},
    {"/",               SYMBOL,         MULT,       SECOND,             53},

    {"^",               SYMBOL,         MULT,       THIRD,              60},

    {"++",              SYMBOL,         SOLO,       FOURTH,             70},
    {"--",              SYMBOL,         SOLO,       FOURTH,             71},
    {"!",               SYMBOL,         SOLO,       FOURTH,             72}, 


    {"(",               SYMBOL,         SOLO,       OP_BR_ONE,          0},
    {")",               SYMBOL,         SOLO,       CL_BR_ONE,          0},
    {"{",               SYMBOL,         SOLO,       OP_BR_TWO,          0},
    {"}",               SYMBOL,         SOLO,       CL_BR_TWO,          0},
    {"[",               SYMBOL,         SOLO,       OP_BR_THREE,        0},
    {"]",               SYMBOL,         SOLO,       CL_BR_THREE,        0},
};
const size_t SIZE_OF_OPERATORS = sizeof(ArrayOperators)/sizeof(ArrayOperators[0]);

#endif
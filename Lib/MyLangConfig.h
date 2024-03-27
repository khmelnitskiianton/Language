#ifndef MY_LANG_CONFIG_H
#define MY_LANG_CONFIG_H

//Config of my language 
//includes in ../FrontEnd/hdr/parser.h

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
    EnumOperClass   Class; //FOR PARSER CATEGORY
    size_t          Id;//FOR STANDART OF TREE
} Operator_t;

constexpr Operator_t ArrayOperators[] = {
    {"func_def",        LETTER,         FUNC_DEF,           10},
    {"func_def_help",   LETTER,         FUNC_DEF_HELP,      11},
    {"call",            LETTER,         CALL,               12},
    {"return",          LETTER,         RETURN,             13},

    {";",               SYMBOL,         DIVIDER,            20},
    {",",               SYMBOL,         DIVIDER_ARG,        21},
    {"=",               SYMBOL,         EQUAL,              22},   

    {"var",             LETTER,         VAR,                100},
    {"void",            LETTER,         VOID,               101},
    {"if",              LETTER,         IF,                 102},
    {"while",           LETTER,         WHILE,              103},
    {"else",            LETTER,         ELSE,               104},
    {"break",           LETTER,         BREAK,              105},
    {"continue",        LETTER,         CONTINUE,           106},

    {">",               SYMBOL,         ZERO,               30},
    {"<",               SYMBOL,         ZERO,               31},
    {">=",              SYMBOL,         ZERO,               32},
    {"<=",              SYMBOL,         ZERO,               33},
    {"!=",              SYMBOL,         ZERO,               34},
    {"==",              SYMBOL,         ZERO,               35},

    {"+",               SYMBOL,         FIRST,              40},
    {"-",               SYMBOL,         FIRST,              41},
    {"||",              SYMBOL,         FIRST,              42},

    {"&&",              SYMBOL,         SECOND,             50},
    {"%",               SYMBOL,         SECOND,             51},
    {"*",               SYMBOL,         SECOND,             52},
    {"/",               SYMBOL,         SECOND,             53},

    {"^",               SYMBOL,         THIRD,              60},

    {"++",              SYMBOL,         FOURTH,             70},
    {"--",              SYMBOL,         FOURTH,             71},
    {"!",               SYMBOL,         FOURTH,             72}, 


    {"(",               SYMBOL,         OP_BR_ONE,          0},
    {")",               SYMBOL,         CL_BR_ONE,          0},
    {"{",               SYMBOL,         OP_BR_TWO,          0},
    {"}",               SYMBOL,         CL_BR_TWO,          0},
    {"[",               SYMBOL,         OP_BR_THREE,        0},
    {"]",               SYMBOL,         CL_BR_THREE,        0},
};
const size_t SIZE_OF_OPERATORS = sizeof(ArrayOperators)/sizeof(ArrayOperators[0]);

#endif
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

enum EnumOperArgType{
    UNARY_ARG,
    BINARY_ARG,
};

typedef struct Operator {
    const char*     Name;
    int             (*Operation)(FILE* FileProc, int label_counter);
    EnumOperArgType ArgType;
    EnumOperType    Type;  
    EnumOperNum     Comp;
    EnumOperClass   Class;  //FOR PARSER CATEGORY
    size_t          Id;     //FOR STANDART OF TREE
} Operator_t;
 
int     _ABO       (FILE* FileProc, int label_counter);
int     _BEL       (FILE* FileProc, int label_counter);
int     _ABOEQU    (FILE* FileProc, int label_counter);
int     _BELEQU    (FILE* FileProc, int label_counter);
int     _NEQU      (FILE* FileProc, int label_counter);
int     _EQU       (FILE* FileProc, int label_counter);
int     _ADD       (FILE* FileProc, int label_counter);
int     _SUB       (FILE* FileProc, int label_counter);
int     _OR        (FILE* FileProc, int label_counter);
int     _AND       (FILE* FileProc, int label_counter);
int     _MOD       (FILE* FileProc, int label_counter);
int     _MUL       (FILE* FileProc, int label_counter);
int     _DIV       (FILE* FileProc, int label_counter);
int     _POW       (FILE* FileProc, int label_counter);
int     _UADD      (FILE* FileProc, int label_counter);
int     _USUB      (FILE* FileProc, int label_counter);
int     _NOT       (FILE* FileProc, int label_counter);

constexpr Operator_t ArrayOperators[] = {
    {"func_def",            NULL,       BINARY_ARG,         LETTER,         MULT,       FUNC_DEF,           10},
    {"func_def_help",       NULL,       BINARY_ARG,         LETTER,         MULT,       FUNC_DEF_HELP,      11},
    {"call",                NULL,       BINARY_ARG,         LETTER,         MULT,       CALL,               12},
    {"return",              NULL,       BINARY_ARG,         LETTER,         MULT,       RETURN,             13},
    
    {";",                   NULL,       BINARY_ARG,         SYMBOL,         SOLO,       DIVIDER,            20},
    {",",                   NULL,       BINARY_ARG,         SYMBOL,         SOLO,       DIVIDER_ARG,        21},
    
    {"var",                 NULL,       BINARY_ARG,         LETTER,         MULT,       VAR,                100},
    {"void",                NULL,       BINARY_ARG,         LETTER,         MULT,       VOID,               101},
    {"if",                  NULL,       BINARY_ARG,         LETTER,         MULT,       IF,                 102},
    {"while",               NULL,       BINARY_ARG,         LETTER,         MULT,       WHILE,              103},
    {"else",                NULL,       BINARY_ARG,         LETTER,         MULT,       ELSE,               104},
    {"break",               NULL,       BINARY_ARG,         LETTER,         MULT,       BREAK,              105},
    {"continue",            NULL,       BINARY_ARG,         LETTER,         MULT,       CONTINUE,           106},
    
    {">=",                  _ABOEQU,    BINARY_ARG,         SYMBOL,         SOLO,       ZERO,               32},
    {"<=",                  _BELEQU,    BINARY_ARG,         SYMBOL,         SOLO,       ZERO,               33},
    {"!=",                  _NEQU,      BINARY_ARG,         SYMBOL,         SOLO,       ZERO,               34},
    {"==",                  _EQU,       BINARY_ARG,         SYMBOL,         SOLO,       ZERO,               35},
    {"--",                  _USUB,      UNARY_ARG,          SYMBOL,         SOLO,       FOURTH,             71},
    {"||",                  _OR,        BINARY_ARG,         SYMBOL,         SOLO,       FIRST,              42},
    {"&&",                  _AND,       BINARY_ARG,         SYMBOL,         SOLO,       SECOND,             50},

    {">",                   _ABO,       BINARY_ARG,         SYMBOL,         SOLO,       ZERO,               30},
    {"<",                   _BEL,       BINARY_ARG,         SYMBOL,         SOLO,       ZERO,               31},
    {"+",                   _ADD,       BINARY_ARG,         SYMBOL,         SOLO,       FIRST,              40},
    {"-",                   _SUB,       BINARY_ARG,         SYMBOL,         SOLO,       FIRST,              41},
    {"%",                   _MOD,       BINARY_ARG,         SYMBOL,         SOLO,       SECOND,             51},
    {"*",                   _MUL,       BINARY_ARG,         SYMBOL,         SOLO,       SECOND,             52},
    {"/",                   _DIV,       BINARY_ARG,         SYMBOL,         SOLO,       SECOND,             53},
    {"!",                   _NOT,       BINARY_ARG,         SYMBOL,         SOLO,       FOURTH,             72},
    {"=",                   NULL,       BINARY_ARG,         SYMBOL,         SOLO,       EQUAL,              22},    

    {"(",                   NULL,       BINARY_ARG,         SYMBOL,         SOLO,       OP_BR_ONE,          0},
    {")",                   NULL,       BINARY_ARG,         SYMBOL,         SOLO,       CL_BR_ONE,          0},
    {"{",                   NULL,       BINARY_ARG,         SYMBOL,         SOLO,       OP_BR_TWO,          0},
    {"}",                   NULL,       BINARY_ARG,         SYMBOL,         SOLO,       CL_BR_TWO,          0},
    {"[",                   NULL,       BINARY_ARG,         SYMBOL,         SOLO,       OP_BR_THREE,        0},
    {"]",                   NULL,       BINARY_ARG,         SYMBOL,         SOLO,       CL_BR_THREE,        0},
};
const size_t SIZE_OF_OPERATORS = sizeof(ArrayOperators)/sizeof(ArrayOperators[0]);

#endif
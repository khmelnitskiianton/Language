#ifndef MY_LANG_CONFIG_H
#define MY_LANG_CONFIG_H

//Config of my language 
#define INOUT_FUNCTIONS "global main\n"                                 \
                        "\n"                                            \
                        "extern print\n"                                \
                        "extern input\n"                                \
                        "extern error_end\n"                            \
                        "extern __processing_unassigned_var__\n"        \
                        "\n"                                            \
                        "extern sqrt\n"                                 \
                        "extern pow\n"

#define UNUSED(x) (void)(x)

const int INACCURACY = 100;

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
    int             (*Operation)(FILE* FileProc, Node_t* CurrentNode, int label_counter);
    EnumOperArgType ArgType;
    EnumOperType    Type;  
    EnumOperNum     Comp;
    EnumOperClass   Class;  //FOR PARSER CATEGORY
    size_t          Id;     //FOR STANDART OF TREE
} Operator_t;
 
int     _ABO       (FILE* FileProc, Node_t* CurrentNode , int label_counter);
int     _BEL       (FILE* FileProc, Node_t* CurrentNode , int label_counter);
int     _ABOEQU    (FILE* FileProc, Node_t* CurrentNode , int label_counter);
int     _BELEQU    (FILE* FileProc, Node_t* CurrentNode , int label_counter);
int     _NEQU      (FILE* FileProc, Node_t* CurrentNode , int label_counter);
int     _EQU       (FILE* FileProc, Node_t* CurrentNode , int label_counter);
int     _ADD       (FILE* FileProc, Node_t* CurrentNode , int label_counter);
int     _SUB       (FILE* FileProc, Node_t* CurrentNode , int label_counter);
int     _OR        (FILE* FileProc, Node_t* CurrentNode , int label_counter);
int     _AND       (FILE* FileProc, Node_t* CurrentNode , int label_counter);
int     _MOD       (FILE* FileProc, Node_t* CurrentNode , int label_counter);
int     _MUL       (FILE* FileProc, Node_t* CurrentNode , int label_counter);
int     _DIV       (FILE* FileProc, Node_t* CurrentNode , int label_counter);
int     _POW       (FILE* FileProc, Node_t* CurrentNode , int label_counter);
int     _UADD      (FILE* FileProc, Node_t* CurrentNode , int label_counter);
int     _USUB      (FILE* FileProc, Node_t* CurrentNode , int label_counter);
int     _NOT       (FILE* FileProc, Node_t* CurrentNode , int label_counter);

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
    {"++",                  _UADD,      UNARY_ARG,          SYMBOL,         SOLO,       FOURTH,             71},
    {"||",                  _OR,        BINARY_ARG,         SYMBOL,         SOLO,       FIRST,              42},
    {"&&",                  _AND,       BINARY_ARG,         SYMBOL,         SOLO,       SECOND,             50},

    {">",                   _ABO,       BINARY_ARG,         SYMBOL,         MULT,       ZERO,               30},
    {"<",                   _BEL,       BINARY_ARG,         SYMBOL,         MULT,       ZERO,               31},
    {"+",                   _ADD,       BINARY_ARG,         SYMBOL,         MULT,       FIRST,              40},
    {"-",                   _SUB,       BINARY_ARG,         SYMBOL,         MULT,       FIRST,              41},
    {"=",                   NULL,       BINARY_ARG,         SYMBOL,         MULT,       EQUAL,              22},    
    {"!",                   _NOT,       BINARY_ARG,         SYMBOL,         MULT,       FOURTH,             72},
    {"%",                   _MOD,       BINARY_ARG,         SYMBOL,         SOLO,       SECOND,             51},
    {"*",                   _MUL,       BINARY_ARG,         SYMBOL,         SOLO,       SECOND,             52},
    {"/",                   _DIV,       BINARY_ARG,         SYMBOL,         SOLO,       SECOND,             53},

    {"(",                   NULL,       BINARY_ARG,         SYMBOL,         SOLO,       OP_BR_ONE,          0},
    {")",                   NULL,       BINARY_ARG,         SYMBOL,         SOLO,       CL_BR_ONE,          0},
    {"{",                   NULL,       BINARY_ARG,         SYMBOL,         SOLO,       OP_BR_TWO,          0},
    {"}",                   NULL,       BINARY_ARG,         SYMBOL,         SOLO,       CL_BR_TWO,          0},
    {"[",                   NULL,       BINARY_ARG,         SYMBOL,         SOLO,       OP_BR_THREE,        0},
    {"]",                   NULL,       BINARY_ARG,         SYMBOL,         SOLO,       CL_BR_THREE,        0},
};
const size_t SIZE_OF_OPERATORS = sizeof(ArrayOperators)/sizeof(ArrayOperators[0]);

#endif
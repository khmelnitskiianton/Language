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
    DIVIDER,
    DIVIDER_ARG,

    OPEN_BRACKET_ROUND,
    CLOSE_BRACKET_ROUND,
    OPEN_BRACKET_FIGURE,
    CLOSE_BRACKET_FIGURE,
    OPEN_BRACKET_SQUARE,
    CLOSE_BRACKET_SQUARE,
    
    ZERO,  
    FIRST, 
    SECOND,
    THIRD,
    FOURTH,
    EQUAL,
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

enum EnumIndexOper{
    INDEX_SEP,
    INDEX_SEP_ARG,
    OP_BR_ONE,
    CL_BR_ONE,
    OP_BR_TWO,
    CL_BR_TWO,
    OP_BR_THREE,    
    CL_BR_THREE,
    
    INDEX_FUNC_DEF,
    INDEX_FUNC_HEL,
    INDEX_CALL,
    INDEX_RETURN,
    INDEX_VAR,
    INDEX_VOID,

    INDEX_IF,
    INDEX_WHILE,
    INDEX_ELSE,
    INDEX_BREAK,
    INDEX_CONTINUE,

    INDEX_ABOEQU,
    INDEX_BELEQU,
    INDEX_NEQU,
    INDEX_EQU,
    INDEX_OR,
    INDEX_AND,
    INDEX_ABO,
    INDEX_BEL,
    INDEX_ADD,
    INDEX_SUB,
    INDEX_ASSIGN,
    INDEX_NOT,
    INDEX_MOD,
    INDEX_MUL,
    INDEX_DIV,
};

enum EnumOperArgType{
    UNARY_ARG,
    BINARY_ARG,
};

typedef struct Operator {
    const char*     Name;
    int             (*Operation)(BinaryTree_t* myTree, FILE* FileProc, Node_t* CurrentNode, int label_counter, void (*RecEvaluate) (BinaryTree_t* , Node_t*));
    EnumOperArgType ArgType;
    EnumOperType    Type;  
    EnumOperNum     Comp;
    EnumOperClass   Class;  //FOR PARSER CATEGORY
    size_t          Id;     //FOR STANDARD OF TREE
} Operator_t;
 
int     _ABO       (BinaryTree_t* myTree, FILE* FileProc, Node_t* CurrentNode , int label_counter, void (*RecEvaluate) (BinaryTree_t* , Node_t*));
int     _BEL       (BinaryTree_t* myTree, FILE* FileProc, Node_t* CurrentNode , int label_counter, void (*RecEvaluate) (BinaryTree_t* , Node_t*));
int     _ABOEQU    (BinaryTree_t* myTree, FILE* FileProc, Node_t* CurrentNode , int label_counter, void (*RecEvaluate) (BinaryTree_t* , Node_t*));
int     _BELEQU    (BinaryTree_t* myTree, FILE* FileProc, Node_t* CurrentNode , int label_counter, void (*RecEvaluate) (BinaryTree_t* , Node_t*));
int     _NEQU      (BinaryTree_t* myTree, FILE* FileProc, Node_t* CurrentNode , int label_counter, void (*RecEvaluate) (BinaryTree_t* , Node_t*));
int     _EQU       (BinaryTree_t* myTree, FILE* FileProc, Node_t* CurrentNode , int label_counter, void (*RecEvaluate) (BinaryTree_t* , Node_t*));
int     _ADD       (BinaryTree_t* myTree, FILE* FileProc, Node_t* CurrentNode , int label_counter, void (*RecEvaluate) (BinaryTree_t* , Node_t*));
int     _SUB       (BinaryTree_t* myTree, FILE* FileProc, Node_t* CurrentNode , int label_counter, void (*RecEvaluate) (BinaryTree_t* , Node_t*));
int     _OR        (BinaryTree_t* myTree, FILE* FileProc, Node_t* CurrentNode , int label_counter, void (*RecEvaluate) (BinaryTree_t* , Node_t*));
int     _AND       (BinaryTree_t* myTree, FILE* FileProc, Node_t* CurrentNode , int label_counter, void (*RecEvaluate) (BinaryTree_t* , Node_t*));
int     _MOD       (BinaryTree_t* myTree, FILE* FileProc, Node_t* CurrentNode , int label_counter, void (*RecEvaluate) (BinaryTree_t* , Node_t*));
int     _MUL       (BinaryTree_t* myTree, FILE* FileProc, Node_t* CurrentNode , int label_counter, void (*RecEvaluate) (BinaryTree_t* , Node_t*));
int     _DIV       (BinaryTree_t* myTree, FILE* FileProc, Node_t* CurrentNode , int label_counter, void (*RecEvaluate) (BinaryTree_t* , Node_t*));
int     _UADD      (BinaryTree_t* myTree, FILE* FileProc, Node_t* CurrentNode , int label_counter, void (*RecEvaluate) (BinaryTree_t* , Node_t*));
int     _USUB      (BinaryTree_t* myTree, FILE* FileProc, Node_t* CurrentNode , int label_counter, void (*RecEvaluate) (BinaryTree_t* , Node_t*));
int     _NOT       (BinaryTree_t* myTree, FILE* FileProc, Node_t* CurrentNode , int label_counter, void (*RecEvaluate) (BinaryTree_t* , Node_t*));

//WARNING DON'T CHANGE!!!!

constexpr Operator_t ArrayOperators[] = {
    [INDEX_SEP]     = {";",             NULL,       BINARY_ARG,         SYMBOL,         SOLO,       DIVIDER,            20},
    [INDEX_SEP_ARG] = {",",             NULL,       BINARY_ARG,         SYMBOL,         SOLO,       DIVIDER_ARG,        21},
    [OP_BR_ONE]     = {"(",             NULL,       BINARY_ARG,         SYMBOL,         SOLO,       OPEN_BRACKET_ROUND,     0},
    [CL_BR_ONE]     = {")",             NULL,       BINARY_ARG,         SYMBOL,         SOLO,       CLOSE_BRACKET_ROUND,    0},
    [OP_BR_TWO]     = {"{",             NULL,       BINARY_ARG,         SYMBOL,         SOLO,       OPEN_BRACKET_FIGURE,    0},
    [CL_BR_TWO]     = {"}",             NULL,       BINARY_ARG,         SYMBOL,         SOLO,       CLOSE_BRACKET_FIGURE,   0},
    [OP_BR_THREE]   = {"[",             NULL,       BINARY_ARG,         SYMBOL,         SOLO,       OPEN_BRACKET_SQUARE,    0},
    [CL_BR_THREE]   = {"]",             NULL,       BINARY_ARG,         SYMBOL,         SOLO,       CLOSE_BRACKET_SQUARE,   0},

    [INDEX_FUNC_DEF]= {"func_def",      NULL,   BINARY_ARG, LETTER,         MULT,       FUNC_DEF,           10},
    [INDEX_FUNC_HEL]= {"func_def_help", NULL,   BINARY_ARG, LETTER,         MULT,       FUNC_DEF_HELP,      11},
    [INDEX_CALL]    = {"call",          NULL,   BINARY_ARG, LETTER,         MULT,       CALL,               12},
    [INDEX_RETURN]  = {"return",        NULL,   BINARY_ARG, LETTER,         MULT,       RETURN,             13},
    [INDEX_VAR]     = {"var",           NULL,   BINARY_ARG, LETTER,         MULT,       VAR,                100},
    [INDEX_VOID]    = {"void",          NULL,   BINARY_ARG, LETTER,         MULT,       VOID,               101},
    [INDEX_IF]      = {"if",            NULL,   BINARY_ARG, LETTER,         MULT,       IF,                 102},
    [INDEX_WHILE]   = {"while",         NULL,   BINARY_ARG, LETTER,         MULT,       WHILE,              103},
    [INDEX_ELSE]    = {"else",          NULL,   BINARY_ARG, LETTER,         MULT,       ELSE,               104},
    [INDEX_BREAK]   = {"break",         NULL,   BINARY_ARG, LETTER,         MULT,       BREAK,              105},
    [INDEX_CONTINUE]= {"continue",      NULL,   BINARY_ARG, LETTER,         MULT,       CONTINUE,           106},
    
    [INDEX_ABOEQU]  = {">=",      _ABOEQU,    BINARY_ARG,     SYMBOL,         SOLO,       ZERO,               32},
    [INDEX_BELEQU]  = {"<=",      _BELEQU,    BINARY_ARG,     SYMBOL,         SOLO,       ZERO,               33},
    [INDEX_NEQU]    = {"!=",      _NEQU,      BINARY_ARG,     SYMBOL,         SOLO,       ZERO,               34},
    [INDEX_EQU]     = {"==",      _EQU,       BINARY_ARG,     SYMBOL,         SOLO,       ZERO,               35},
    [INDEX_OR]      = {"||",      _OR,        BINARY_ARG,     SYMBOL,         SOLO,       FIRST,              42},
    [INDEX_AND]     = {"&&",      _AND,       BINARY_ARG,     SYMBOL,         SOLO,       SECOND,             50},
    [INDEX_ABO]     = {">",       _ABO,       BINARY_ARG,     SYMBOL,         MULT,       ZERO,               30},
    [INDEX_BEL]     = {"<",       _BEL,       BINARY_ARG,     SYMBOL,         MULT,       ZERO,               31},
    [INDEX_ADD]     = {"+",       _ADD,       BINARY_ARG,     SYMBOL,         SOLO,       FIRST,              40},
    [INDEX_SUB]     = {"-",       _SUB,       BINARY_ARG,     SYMBOL,         SOLO,       FIRST,              41},
    [INDEX_ASSIGN]  = {"=",       NULL,       BINARY_ARG,     SYMBOL,         MULT,       EQUAL,              22},    
    [INDEX_NOT]     = {"!",       _NOT,       BINARY_ARG,     SYMBOL,         MULT,       FOURTH,             72},
    [INDEX_MOD]     = {"%",       _MOD,       BINARY_ARG,     SYMBOL,         SOLO,       SECOND,             51},
    [INDEX_MUL]     = {"*",       _MUL,       BINARY_ARG,     SYMBOL,         SOLO,       SECOND,             52},
    [INDEX_DIV]     = {"/",       _DIV,       BINARY_ARG,     SYMBOL,         SOLO,       SECOND,             53},
};
const size_t SIZE_OF_OPERATORS = sizeof(ArrayOperators)/sizeof(ArrayOperators[0]);

//DEFINES DSL FOR PARSER IN FRONTEND!

#define CHECK_OPER_CLASS(ptr_token, oper_class) ArrayOperators[(ptr_token)->Value.Index].Class == oper_class

#define CHECK_TYPE_OPER(ptr_token) (ptr_token)->Type == OPERATOR
#define CHECK_TYPE_NUM(ptr_token) (ptr_token)->Type == NUMBER
#define CHECK_TYPE_VAR(ptr_token) (ptr_token)->Type == VARIABLE

#define INCREMENT_PTR_TOKEN(ptr_token) (*(ptr_token))++

#endif
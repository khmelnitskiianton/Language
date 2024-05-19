#ifndef CALCULATION_H
#define CALCULATION_H

#define UNUSED(x) (void)(x)

double _add (const double a, const double b);
double _sub (const double a, const double b);
double _mul (const double a, const double b);
double _div (const double a, const double b);
double _pow (const double a, const double b);
double _sin (const double a, const double b);
double _cos (const double a, const double b);
double _tg  (const double a, const double b);
double _ctg (const double a, const double b);
double _ln  (const double a, const double b); 

int FindArithOper   (Node_t* CurrentNode);
int FindFuncOper    (Node_t* CurrentNode, BinaryTree_t* myTree);

typedef struct CalcOper {
    const char* Name;
    double      (*Operation) (const double, const double);
    bool        TypeOperator; //0 - Binary, 1 - Unary TODO: use it
    size_t      Id;
} CalcOper_t;

constexpr CalcOper_t  CalcOperators[] = {
    {"+",       _add,   1,      40},
    {"-",       _sub,   1,      41},
    {"*",       _mul,   1,      52},
    {"/",       _div,   1,      53},
    {"^",       _pow,   1,      60},
    {"sin",     _sin,   0,      0},
    {"cos",     _cos,   0,      0},
    {"tg",      _tg,    0,      0},
    {"ctg",     _ctg,   0,      0},
    {"ln",      _ln,    0,      0},
};

enum EnumOfCalcOpers {
    ADD,
    SUB,
    MUL,
    DIV,
    POW,
    SIN,
    COS,
    TG,
    CTG,
    LN,
};

const size_t SIZE_OF_CALC_OPERATORS = sizeof(CalcOperators)/sizeof(CalcOperators[0]); //размер массива оператора

#endif
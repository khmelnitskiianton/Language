#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "tree.h"
#include "calculation.h"
#include "colors.h"
#include "MyLangConfig.h"
#include "myassert.h"
#include "verificator.h"
#include "dsl.h"

//Founder

int FindArithOper(Node_t* CurrentNode)
{
    for (size_t i = 0; i < SIZE_OF_CALC_OPERATORS; i++)
    {
        if ((CalcOperators[i].Id != 0) && (CalcOperators[i].Id == ArrayOperators[C->Value.Index].Id))
        {
            return (int)i;
        }
    }
    return -1;
}

int FindFuncOper(Node_t* CurrentNode, BinaryTree_t* myTree)
{
    for (size_t i = 0; i < SIZE_OF_CALC_OPERATORS; i++)
    {
        if ((CalcOperators[i].Id == 0)&&(!strncmp(CalcOperators[i].Name, myTree->Variables[C->Value.Index].Name, SIZE_OF_VAR)))
        {
            return (int)i;
        }
    }
    return 0;
}

//Binary Operations
double _add(const double a, const double b)
{
    return a + b;
}

double _sub(const double a, const double b)
{
    return a - b;
}

double _mul(const double a, const double b)
{
    return a * b;
}

double _div(const double a, const double b)
{
    bool check_div_zero = Compare(b,0);
    if(check_div_zero) return NAN;
    return a / b;
}

double _pow(const double a, const double b)
{
    bool check_pow_nan = Compare(b,0);
    if(check_pow_nan) return NAN;
    return pow(a, b);
}

//Unary Operations
double _sin(const double a, const double b)
{
    UNUSED(b);
    return sin(a);
}

double _cos(const double a, const double b)
{
    UNUSED(b);
    return cos(a);
}

double _tg(const double a, const double b)
{
    UNUSED(b);
    bool check_div_zero = Compare(cos(a),0);
    if(check_div_zero) return NAN;
    return sin(a)/cos(a);
}

double _ctg(const double a, const double b)
{
    UNUSED(b);
    bool check_div_zero = Compare(sin(a),0);
    if(check_div_zero) return NAN;
    return cos(a)/sin(a);
}

double _ln(const double a, const double b)
{
    UNUSED(b);
    if (!(a > 0)) return NAN;
    return log(a);
}
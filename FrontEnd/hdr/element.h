#ifndef ELEMENT_H
#define ELEMENT_H

#include <math.h>
#include <limits.h>

typedef double Elem_t;

#define SPECIFIER_SCANF "%lf"
#define SPECIFIER_INIT " %.3lf "
#define SPECIFIER_NUMBER " %.3lf "
#define SPECIFIER_OPERATOR_STR " %s "
#define SPECIFIER_VARIABLE_STR " %s "
#define SPECIFIER_VARIABLE_NUM " %.3lf "

const Elem_t POISON_ELEMENT = NAN;
const double EPSILONE = 1e-5; 

#endif
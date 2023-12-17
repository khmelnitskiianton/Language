#ifndef ERRORS_ENUM_H
#define ERRORS_ENUM_H

enum EnumOfErrors 
{
    #define DEF_ERROR(error, ...) ERR_ ## error,
    #include "errors_codegen.h"
    #undef DEF_ERROR
};
const char* const ArrayOfErrors[] =
{
    #define DEF_ERROR(err, str) str,
    #include "errors_codegen.h"
    #undef DEF_ERROR
};

#endif 
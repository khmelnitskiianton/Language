#ifndef ERRORS_CODEGEN_H
#define ERRORS_CODEGEN_H
//ENUM AND ARRAY OF ERRORS

enum EnumOfErrors 
{
    #define DEF_ERROR(error, ...) ERR_ ## error,
    #include "errors_list.h"
    #undef DEF_ERROR
};
const char* const ArrayOfErrors[] =
{
    #define DEF_ERROR(err, str) str,
    #include "errors_list.h"
    #undef DEF_ERROR
};

#endif 
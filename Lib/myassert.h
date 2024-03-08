#ifndef MYASSERT_H
#define MYASSERT_H

#include <assert.h>
#include "colors.h"
#include "errors_codegen.h"

#define DEBUG_MYASSERT 1

#ifdef DEBUG_MYASSERT
#define MYASSERT(check, error_code, ending)                                                                             \
        do                                                                                                              \
        {                                                                                                               \
            if (!(check))                                                                                               \
            {                                                                                                           \
                fprintf(stderr, MAGENTA ">>>>> Error in code! Error code: %s <<<<<\n", #error_code);                    \
                fprintf(stderr, "In File: %s, In Line: %d, In Function: %s\n",__FILE__,__LINE__,__PRETTY_FUNCTION__);   \
                fprintf(stderr, "In this condition:\t\"%s\"\n\n" RESET, #check);                                        \
                ending;                                                                                                 \
            }                                                                                                           \
        }                                                                                                               \
        while(0);
#else   
#define MYASSERT(check, error_code, ending)
#endif

//===============================================================================================================
//Write Error unclosed
#define USER_ERROR(check, error_code, word, ending)                                                         \
        do                                                                                                  \
        {                                                                                                   \
            if (!(check))                                                                                   \
            {                                                                                               \
                fprintf(stderr, RED ">>>>> Error! %s <<<<<\n" RESET, ArrayOfErrors[error_code]);            \
                fprintf(stderr, MAGENTA "%s" RESET, word);                                                \
                fprintf(stderr, RED "In File: %s, In Line: %d, In Function: %s\n",__FILE__,__LINE__,__PRETTY_FUNCTION__);\
                ending;                                                                                     \
            }                                                                                               \
        }                                                                                                   \
        while(0);

#endif
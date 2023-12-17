#ifndef VERIFICATOR_H
#define VERIFICATOR_H

#include "dump.h"
#include "errors_enum.h"

#define DEBUG_VERIFY 1

#define DUMP_AND_RETURN_ERROR(err)                                  \
    do                                                              \
    {                                                               \
        DUMP_ERROR("ERROR: %s\n", ArrayOfErrors[(int)err]);         \
        return err;                                                 \
    } while (0);                                                    \

#ifdef DEBUG_VERIFY
    #define Verify(object)                              \
    do                                                  \
    {                                                   \
        EnumOfErrors err = _Verify(object);             \
        if (err != ERR_OK)                              \
        {                                               \
            DUMP_AND_RETURN_ERROR(err)                  \
        }                                               \
    } while (0)
#else
    #define Verify(object) do {} while(0)
#endif

EnumOfErrors _Verify(BinaryTree_t* myTree);

#endif
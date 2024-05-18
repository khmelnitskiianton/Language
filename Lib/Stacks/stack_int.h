#ifndef STACK_INT_H
#define STACK_INT_H

#define ASSERT_MYSTACK 1
#define DEBUG_PRINTING 1

#if (ASSERT_MYSTACK != 0)
#define ASSERT_STACK(stk, func)                             \
        if (checkStack(stk, func) != 0)                     \
        {                                                   \
            STACK_DUMP (stk, checkStack(stk, func))         \
        }                                                   
#define STACK_DUMP(stk, error_code) output_error(stk, __FILE__, __LINE__, __PRETTY_FUNCTION__, error_code);
#else   
#define ASSERT_STACK(stk, func)                   
#define STACK_DUMP(stk, error_code)        
#endif

#if (DEBUG_PRINTING != 0)
#define ON_PRINTING(...) __VA_ARGS__
#else
#define ON_PRINTING(...) 
#endif

const int STACK_POISON_ELEMENT_INT = 0xFFFFFFF;

typedef struct Stack {
    int     *data;
    int     size;
    int     capacity; 
    int     ret_value;
    FILE*   file_write;
} StackInt_t;

const int    N_ERRORS = 16;
const int    MULTIPLIER = 2;
const int    MIN_LEN = 10;

int StackIntCtor       (StackInt_t *stk, const char* log_file);
int StackIntDtor       (StackInt_t* stk);
int push_int           (StackInt_t* stk, int value);
int pop_int            (StackInt_t* stk, int* ret_value);

#endif
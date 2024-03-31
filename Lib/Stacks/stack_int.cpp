#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "tree.h"
#include "myassert.h"
#include "creator.h"
#include "stack_int.h"

static int*    expansion           (StackInt_t *stk);
static int*    comprassion         (StackInt_t *stk);
static int     mem_poison          (void* memptr, int num);
static int     checkStack          (StackInt_t *stk, const char* func);
static int     output_error        (StackInt_t *stk, const char* file, const size_t line, const char* pretty_function, int error_code);
static int     printing_stack      (StackInt_t* stk, const char* file, const size_t line, const char* pretty_function);
static size_t  size_data           (StackInt_t* stk);

int StackIntCtor (StackInt_t *stk, const char* log_file)
{
    stk -> file_write = OpenFile(log_file, "w");
    stk -> size = 0; 
    stk -> capacity  = 1;
    stk -> ret_value = 1;
    if ((stk -> data = (int*) calloc (1, size_data(stk))) == nullptr)
    {   
        fprintf(stdout,            "ERROR IN INITIALIZATION OF CALLOC");
        fprintf(stk -> file_write, "ERROR IN INITIALIZATION OF CALLOC");  
        abort();
    }
    ASSERT_STACK(stk, __PRETTY_FUNCTION__)
    return 1;
}

int StackIntDtor (StackInt_t *stk)
{
    ASSERT_STACK(stk, __PRETTY_FUNCTION__)
    free(stk -> data);
    CloseFile(stk -> file_write);
    stk -> data = NULL;
    stk = NULL;
    return 1;
}

int push_int (StackInt_t *stk, int value)
{ 
    ASSERT_STACK(stk, __PRETTY_FUNCTION__)
    if ((stk -> size) == (stk -> capacity))
    {
        (stk -> data) = expansion (stk);
    }
    *((stk -> data) + (stk -> size)) = value;
    (stk -> size)++; 
    ASSERT_STACK(stk, __PRETTY_FUNCTION__)
ON_PRINTING(
    printing_stack (stk, __FILE__, __LINE__, __PRETTY_FUNCTION__);
)
    return 1;
}

int pop_int (StackInt_t *stk, int* ret_value)
{
    if (!stk) 
    {
        ASSERT_STACK(stk, __PRETTY_FUNCTION__)
    }
    else if (ret_value == NULL) 
    {
        stk -> ret_value = 0;
        ASSERT_STACK(stk, __PRETTY_FUNCTION__)
    }
    else 
    {
        stk -> ret_value = 1;
        ASSERT_STACK(stk, __PRETTY_FUNCTION__)
    }

    int save_value = STACK_POISON_ELEMENT_INT;
    (stk -> size)--;
    save_value = *((stk -> data) + (stk -> size));  
    if (((stk -> size) > 0) && ((stk -> size)*MULTIPLIER < (stk -> capacity)) && ((stk -> capacity) > MIN_LEN))
    {
        (stk -> data) = comprassion (stk);
    }
    *((stk -> data) + (stk -> size)) = STACK_POISON_ELEMENT_INT;
    *ret_value = save_value;
    ASSERT_STACK(stk, "")
ON_PRINTING(
    printing_stack (stk, __FILE__, __LINE__, __PRETTY_FUNCTION__);
)
    return 1;
}

//=======================================================================================
//=======================================================================================


static int* expansion (StackInt_t *stk)
{
    void* new_place = nullptr;
    (stk -> capacity) = (stk -> capacity) * MULTIPLIER;                                                                                      
    if ((new_place = realloc ((stk -> data), sizeof(int)*((size_t) stk -> capacity))) == nullptr)
    {   
        fprintf(stdout,            "ERROR IN RECALLOC IN %s", __PRETTY_FUNCTION__);
        fprintf(stk -> file_write, "ERROR IN RECALLOC IN %s", __PRETTY_FUNCTION__);  
        abort();
    }
    mem_poison ((void*) ((char*) new_place + sizeof(int)*((size_t) stk -> size)), (stk -> capacity)-(stk -> size));                        
    return (int*) new_place;
}

static int* comprassion (StackInt_t *stk)
{
    void* new_place = nullptr; 
    (stk -> capacity) = (stk -> capacity) / MULTIPLIER;
    if ((new_place = realloc ((stk -> data), sizeof(int)*((size_t) stk -> capacity))) == nullptr)
    {
        fprintf(stdout,            "ERROR IN RECALLOC IN %s", __PRETTY_FUNCTION__);
        fprintf(stk -> file_write, "ERROR IN RECALLOC IN %s", __PRETTY_FUNCTION__);  
        abort();
    }
    return (int*) new_place;
}

static int mem_poison (void* memptr, int num)
{
    for (int i = 0; i < num; i++)
    {
        *(((int*) memptr) + i) = STACK_POISON_ELEMENT_INT;
    }
    return 1;
}


static int checkStack (StackInt_t *stk, const char* func)
{
    int error_code = 0;
    
    if (!stk)                    
    {
        error_code |= 1<<0;
        return error_code;
    }
    if (!(stk -> file_write))                    
    {
        error_code |= 1<<1;
        return error_code;
    }

    if (!(stk -> data))
    {
        error_code |= 1<<4;
        return error_code;
    }

    if ((strcmp(func, "int pop(StackInt_t*, int*)") == 0)&&(!(stk -> ret_value)))
    {
        error_code |= 1<<5;
        return error_code;
    }
    if ((stk -> capacity) <= 0)             
    {
        error_code |= 1<<6;
        return error_code;
    }
        if ((stk -> size) < 0)                  
    {
        error_code |= 1<<7;
        return error_code;
    }
    if ((stk -> size) > (stk -> capacity))  
    {
        error_code |= 1<<8;
        return error_code;
    }
    if ((strcmp(func, "int pop(StackInt_t*, int*)") == 0)&&((stk -> size) == 0)) 
    {
        error_code |= 1<<9;
        return error_code;
    }
    return error_code;
}

static int output_error (StackInt_t *stk, const char* file, const size_t line, const char* pretty_function, int error_code)
{   
    const char* mass_of_errors[N_ERRORS] = {
        "ADDRESS OF STRUCTURE == NULL. OUTPUT IN TERMINAL",
        "ADDRESS OF FILE_WRITE == NULL",
        "ADDRESS OF LEFT CANARY IN DATA == NULL",
        "ADDRESS OF RIGHT CANARY IN DATA == NULL",
        "ADDRESS OF ARRAY IN STRUCTURE == NULL",
        "ADDRESS OF RETURNING ARGUMENT IN POP == NULL",
        "CAPACITY <= 0",
        "SIZE < 0",
        "SIZE > CAPACITY",
        "POP() BUT DATA EMPTY",
        "LEFT CANARY IN STRUCT IS DEAD. ANAL PENETRATION IN STRUCT",
        "RIGHT CANARY IN STRUCT IS DEAD. ANAL PENETRATION IN STRUCT",
        "LEFT CANARY IN DATA IS DEAD. ANAL PENETRATION IN DATA",
        "RIGHT CANARY IN DATA IS DEAD. ANAL PENETRATION IN DATA",
        "HASH BROKE. ANAL PENETRATION IN STRUCT",
        "HASH BROKE. ANAL PENETRATION IN DATA"
    };
    int z = error_code;
    unsigned long bin_error = 0;
    int element = 0;
    int fatal_error = 0;
    printf("<<<<<!!!!YOU HAVE ERROR.CHECK OUTPUT.TXT OR TERMINAL!!!!>>>>>>>\n");
    if (z == 1)
    {
        printf("\n<<<<<<<<<<<<<<<YOU HAVE ERROR>>>>>>>>>>>>>>>>>\n"
            "Stack[%p] called from %s (string: %lu) in function %s\n"
            "1: [%s]\n", stk, file, line, pretty_function,mass_of_errors[0]);
        abort();
    }
    if (z == 2)
    {
        printf("\n<<<<<<<<<<<<<<<YOU HAVE ERROR>>>>>>>>>>>>>>>>>\n"
            "Stack[%p] called from %s (string: %lu) in function %s\n"
            "0: [OK]\n"
            "1: [%s]\n", stk, file, line, pretty_function,mass_of_errors[1]);
        abort();
    }

    fprintf(stk -> file_write, "\n<<<<<<<<<<<<<<<YOU HAVE ERROR>>>>>>>>>>>>>>>>>\n");
    fprintf(stk -> file_write, "\nERROR:\n");

    while (z > 0)
    {
        if (z % 2)
        {
            bin_error += (size_t) 1<<(element);
            fprintf(stk -> file_write, "1: [%s]\n", mass_of_errors[element]);
            if(element != 15) fatal_error = 1;
        }
        else
        {
            fprintf(stk -> file_write, "0: [OK]\n");
        }
        z = z / 2;
        element++;
    }
    fprintf(stk -> file_write,"ERROR CODE: [%lu]\n\n", bin_error);
    
    if (!fatal_error)
    {
    fprintf (stk -> file_write,"Stack[%p] called from %s (string: %lu) in function %s\n"
    //      "\t\t%s             \n"
            "{                  \n"
            "\tsize        = %d \n"
            "\tcapacity    = %d \n"
            "\tdata[%p]         \n"
            "\t{                \n", stk, file, line, pretty_function, stk -> size, stk -> capacity, stk -> data);

    for (ssize_t i = 0; (i < (stk -> capacity)); i++)
    {
        if (i == stk -> size)                               fprintf(stk -> file_write,"\t >[%ld] = %d<\n", i, *((stk -> data) + i));
        else if (*((stk -> data) + i) != STACK_POISON_ELEMENT_INT)    fprintf(stk -> file_write,"\t #[%ld] = %d\n", i, *((stk -> data) + i));
        else                                                fprintf(stk -> file_write,"\t @[%ld] = %d(POISON)\n", i, *((stk -> data) + i));
    }
    
    fprintf (stk -> file_write,"\t} \n"
            "}   \n");
    }
    CloseFile(stk -> file_write);
    abort();
    return 1;
}

static int printing_stack (StackInt_t* stk, const char* file, const size_t line, const char* pretty_function)
{
    fprintf (stk -> file_write,"\nStack[%p] called from %s (string: %lu) in function %s\n"
            "{                  \n"
            "\tsize        = %d \n"
            "\tcapacity    = %d \n"
            "\tdata[%p]         \n"
            "\t{                \n", stk, file, line, pretty_function, stk -> size, stk -> capacity, stk -> data);

    for (ssize_t i = 0; (i < (stk -> capacity)); i++)
    {
        if (i == (stk -> size))                             fprintf(stk -> file_write,"\t >[%ld] = %d<\n", i, *((stk -> data) + i));
        else if (*((stk -> data) + i) != STACK_POISON_ELEMENT_INT)    fprintf(stk -> file_write,"\t #[%ld] = %d\n", i, *((stk -> data) + i));
        else                                                fprintf(stk -> file_write,"\t @[%ld] = %d(POISON)\n", i, *((stk -> data) + i));
    }
    
    fprintf (stk -> file_write,"\t} \n"
            "}   \n");
    return 1;
}

static size_t size_data (StackInt_t* stk)
{
    return sizeof(int)*((size_t) (stk -> capacity));
}
 
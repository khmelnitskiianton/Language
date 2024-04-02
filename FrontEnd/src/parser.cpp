#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "tree.h"
#include "tokenizer.h"
#include "parser.h"
#include "colors.h"
#include "log.h"
#include "dsl.h"
#include "myassert.h"
#include "verificator.h"

/*
    =================================================
    |MAKE RECURSIVE DESCENT AND WRITING TREE TO FILE|
    =================================================
*/

//=============================================================================================================

static Node_t* GetCommon            (Token_t** PtrCurrentToken);
static Node_t* GetNumber            (Token_t** PtrCurrentToken);
static Node_t* GetVariable          (Token_t** PtrCurrentToken);
static Node_t* GetZeroPriority      (Token_t** PtrCurrentToken);
static Node_t* GetFirstPriority     (Token_t** PtrCurrentToken);
static Node_t* GetSecondPriority    (Token_t** PtrCurrentToken);
static Node_t* GetThirdPriority     (Token_t** PtrCurrentToken);
static Node_t* GetFourthPriority    (Token_t** PtrCurrentToken); 
static Node_t* GetLastPriority      (Token_t** PtrCurrentToken);
static Node_t* GetAssignment        (Token_t** PtrCurrentToken);
static Node_t* GetFuncLoopEnd       (Token_t** PtrCurrentToken);
static Node_t* GetCondition         (Token_t** PtrCurrentToken);
static Node_t* GetLoop              (Token_t** PtrCurrentToken);
static Node_t* GetFuncCall          (Token_t** PtrCurrentToken);
static Node_t* GetFuncDef           (Token_t** PtrCurrentToken);
static Node_t* GetType              (Token_t** PtrCurrentToken);

static int      FindOper        (const Token_t* CurrentToken, EnumOperClass CurrentClass);
static Node_t*  DiffCreateNode  (EnumOfType NewType, NodeValue_t NewValue, Node_t* LeftNode, Node_t* RightNode);

const int       NO_FIND = -1;
static size_t   amount_nodes = 0;
static bool     syntax_error = 0;

//=============================================================================================================

/*
    ======================================
    |FUNCTIONS FOR MAKE RECURSIVE DESCENT|
    ======================================
*/

Node_t* GetMain(Tokens_t* myTokens, BinaryTree_t* myTree)
{
    Token_t* current_token = myTokens->Data;
    if (current_token->Type == END) return NULL; //TREE FOR EMPTY CODE

    Node_t* new_node  = NULL;
    Node_t* copy_node = NULL;
    int index = 0;
    for (int i = 0; i < (int) SIZE_OF_OPERATORS; i++)
    {
        if (ArrayOperators[i].Class == DIVIDER) {index = i; break;}
    }
    new_node = GetCommon(&current_token);
    while (current_token->Type != END)
    {
        copy_node = new_node;
        new_node = GetCommon(&current_token);
        if ((current_token->Type == OPERATOR) && ((ArrayOperators[current_token->Value.Index].Class == DIVIDER)||(ArrayOperators[current_token->Value.Index].Class == CL_BR_TWO)))
        {
            new_node = OPR(index, copy_node, new_node);
            // char log_buffer[SIZE_OF_LOG_BUFFER] = {};
            // snprintf(log_buffer, SIZE_OF_LOG_BUFFER, "Node|PTR:%p|Type:OPERATOR|VAL:%d|", right_node, (*PT)->Value.Index);
            // PrintLogText(log_buffer);
            current_token++;
        }
        else
        {
            printf("\n%d %d\n",current_token->Type, current_token->Value.Index);
            USER_ERROR(0, ERR_NO_DIVIDER, ArrayOperators[current_token->Value.Index].Name, syntax_error = 1)
            if (syntax_error)
            {
                if (!new_node) RecFree(new_node);
                return NULL;
            }
        }
    }
    myTree->Size = amount_nodes;
    return new_node;
}

static Node_t* GetCommon(Token_t** PtrCurrentToken)
{
    if (syntax_error) 
    {
        return NULL;
    }
    Node_t* NewNode = NULL;
    if ((NewNode = GetZeroPriority(PT)) != NULL)    return NewNode;
    if ((NewNode = GetAssignment(PT))   != NULL)    return NewNode;
    if ((NewNode = GetFuncLoopEnd(PT))  != NULL)    return NewNode;
    if ((NewNode = GetCondition(PT))    != NULL)    return NewNode;
    if ((NewNode = GetLoop(PT))         != NULL)    return NewNode; 
    if ((NewNode = GetFuncCall(PT))     != NULL)    return NewNode;
    if ((NewNode = GetFuncDef(PT))      != NULL)    return NewNode;
    return NULL;
}

static Node_t* GetNumber(Token_t** PtrCurrentToken)
{
    MYASSERT((*PT)->Type == NUMBER, ERR_BAD_TOKEN, return NULL)
    if (syntax_error) 
    {
        return NULL;
    }
    Node_t* new_node = NUM((*PT)->Value.Number);
    (*PT)++;
    return new_node;
}

static Node_t* GetVariable(Token_t** PtrCurrentToken) 
{
    MYASSERT((*PT)->Type == VARIABLE, ERR_BAD_TOKEN, return NULL)
    if (syntax_error) 
    {
        return NULL;
    } 
    Node_t* new_node = VAR((*PT)->Value.Index);
    (*PT)++;
    return new_node;
}

static Node_t* GetZeroPriority(Token_t** PtrCurrentToken) 
{
    if (syntax_error) 
    {
        return NULL;
    }
    Node_t*  left_value   = GetFirstPriority(PT);
    if (left_value == NULL) return NULL;
    Node_t*  right_value  = NULL;

    int index = 0;
    while ((index = FindOper(*PT, ZERO)) != NO_FIND)
    {
        (*PT)++;
        right_value = GetFirstPriority(PT);
        left_value = OPR(index, left_value, right_value);
    }
    if (syntax_error) 
    {
        RecFree(left_value);
        return NULL;
    }
    return left_value;
}

static Node_t* GetFirstPriority(Token_t** PtrCurrentToken) 
{
    if (syntax_error) 
    {
        return NULL;
    }
    Node_t* left_value  = GetSecondPriority(PT);
    Node_t* right_value = NULL;

    int index = 0;
    while ((index = FindOper(*PT, FIRST)) != NO_FIND)
    {
        (*PT)++;
        right_value = GetSecondPriority(PT);
        left_value  = OPR(index, left_value, right_value);
    }
    if (syntax_error) 
    {
        RecFree(left_value);
        return NULL;
    }
    return left_value;
}

static Node_t* GetSecondPriority(Token_t** PtrCurrentToken) 
{
    if (syntax_error) 
    {
        return NULL;
    }
    Node_t* left_value  = GetThirdPriority(PT);
    Node_t* right_value = NULL;

    int index = 0;
    while ((index = FindOper(*PT, SECOND)) != NO_FIND)
    {
        (*PT)++;
        right_value = GetThirdPriority(PT);
        left_value  = OPR(index, left_value, right_value);
    }
    if (syntax_error) 
    {
        RecFree(left_value);
        return NULL;
    }
    return left_value;
}

static Node_t* GetThirdPriority(Token_t** PtrCurrentToken) 
{
    if (syntax_error) 
    {
        return NULL;
    }
    Node_t* left_value  = GetFourthPriority(PT);
    Node_t* right_value = NULL;

    int index = 0;
    while ((index = FindOper(*PT, THIRD)) != NO_FIND)
    {
        (*PT)++;
        right_value = GetFourthPriority(PT);
        left_value  = OPR(index, left_value, right_value);
    }
    if (syntax_error) 
    {
        RecFree(left_value);
        return NULL;
    }
    return left_value;
}

static Node_t* GetFourthPriority(Token_t** PtrCurrentToken)
{
    if (syntax_error) 
    {
        return NULL;
    }
    Node_t* left_value  = GetLastPriority(PT);
    Node_t* right_value = NULL;

    int index = 0;
    while ((index = FindOper(*PT, FOURTH)) != NO_FIND)
    {
        (*PT)++;
        right_value = GetLastPriority(PT);
        left_value = OPR(index, left_value, right_value);
    }
    if (syntax_error) 
    {
        RecFree(left_value);
        return NULL;
    }
    return left_value;
}

static Node_t* GetLastPriority(Token_t** PtrCurrentToken) 
{
    if (syntax_error) 
    {
        return NULL;
    }
    Node_t* val = NULL;
    if (((*PT)->Type == OPERATOR) && (ArrayOperators[(*PT)->Value.Index].Class == OP_BR_ONE))
    {
        (*PT)++;
        val = GetZeroPriority(PT);
        bool check_close_bracket = ((*PT)->Type == OPERATOR) && (ArrayOperators[(*PT)->Value.Index].Class == CL_BR_ONE);
        USER_ERROR(check_close_bracket, ERR_NO_CLOSE_BRACKET, ArrayOperators[(*PT)->Value.Index].Name, syntax_error = 1)
        if (syntax_error)
        {
            RecFree(val);
            return NULL;
        }
        (*PT)++;
        return val;
    }
    if ((*PT)->Type == NUMBER)
    {
        return GetNumber(PT);
    }

    if ((val = GetFuncCall(PT)) != NULL) 
    {
        if (syntax_error) 
        {
            RecFree(val);
            return NULL;
        }
        return val;
    }

    if (syntax_error)
    {
        RecFree(val);
        return NULL;
    }

    if ((*PT)->Type == VARIABLE)
    {
        return GetVariable(PT);
    }

    if (syntax_error) 
    {
        RecFree(val);
        return NULL;
    }

    return val;
}

static Node_t* GetAssignment(Token_t** PtrCurrentToken)
{
    if (syntax_error) 
    {
        return NULL;
    }
    Token_t* current_token = *PT;
    if (((current_token)->Type == OPERATOR)&&(ArrayOperators[(current_token)->Value.Index].Class == VAR)&&((++current_token)->Type == VARIABLE)&&((++current_token)->Type == OPERATOR)&&(ArrayOperators[(current_token)->Value.Index].Class != OP_BR_ONE))
    {
        int index_var = (*PT)->Value.Index;
        (*PT)++;
        Node_t* left_node  = GetVariable(PT);
        Node_t* right_node = NULL;
        Node_t* new_node = NULL;
        if (((*PT)->Type == OPERATOR) && (ArrayOperators[(*PT)->Value.Index].Class == EQUAL))
        {
            int index_equal = (*PT)->Value.Index;
            (*PT)++;
            right_node = GetZeroPriority(PT);
            right_node = OPR(index_equal, left_node, right_node);
            if (syntax_error)
            {
                RecFree(right_node);
                return NULL;
            }
        }
        else
        {
            right_node = left_node;
        }
        new_node = OPR(index_var, NULL, right_node);
        if (syntax_error) 
        {
            RecFree(new_node);
            return NULL;
        }
        return new_node;
    }
    return NULL;
}

static Node_t* GetFuncLoopEnd(Token_t** PtrCurrentToken)
{
    if (syntax_error) 
    {
        return NULL;
    }
    int index = (*PT)->Value.Index;
    if (((*PT)->Type == OPERATOR) && (ArrayOperators[index].Class == BREAK))    
    {   
        (*PT)++;
        return OPR(index, NULL, NULL);
    }
    if (((*PT)->Type == OPERATOR) && (ArrayOperators[index].Class == CONTINUE)) 
    {
        (*PT)++;
        return OPR(index, NULL, NULL);
    }
    if (((*PT)->Type == OPERATOR) && (ArrayOperators[index].Class == RETURN))
    {
        (*PT)++;
        Node_t* left_node = NULL;

        if (((*PT)->Type == OPERATOR) && (ArrayOperators[(*PT)->Value.Index].Class == DIVIDER))
        {
            return OPR(index, NULL, NULL);
        }
        else 
        {
            left_node = GetZeroPriority(PT);
            (*PT)++;
            return OPR(index, left_node, NULL);
        }
    }
    return NULL;
}

static Node_t* GetCondition(Token_t** PtrCurrentToken)
{
    if (syntax_error) 
    {
        return NULL;
    }
    int index_if = (*PT)->Value.Index;
    Node_t* left_node   = NULL;
    Node_t* right_node  = NULL;
    Node_t* copy_node   = NULL;
    Node_t* middle_node = NULL;

    int index_div = 0;
    for (int i = 0; i < (int) SIZE_OF_OPERATORS; i++)
    {
        if (ArrayOperators[i].Class == DIVIDER) {index_div = i; break;}
    }

    if (((*PT)->Type == OPERATOR) && (ArrayOperators[index_if].Class == IF))
    {
        (*PT)++;
        if (((*PT)->Type == OPERATOR) && (ArrayOperators[(*PT)->Value.Index].Class == OP_BR_ONE))
        {
            (*PT)++;
            left_node = GetZeroPriority(PT);
        }
        else
        {
            USER_ERROR(0, ERR_NO_OPEN_BRACKET, ArrayOperators[(*PT)->Value.Index].Name, syntax_error = 1)
            return NULL;    
        }
        bool check_close_bracket = ((*PT)->Type == OPERATOR) && (ArrayOperators[(*PT)->Value.Index].Class == CL_BR_ONE);
        USER_ERROR(check_close_bracket, ERR_NO_CLOSE_BRACKET, ArrayOperators[(*PT)->Value.Index].Name, syntax_error = 1)
        (*PT)++;
        if (syntax_error) 
        {
            RecFree(left_node);
            return NULL;
        }
        if (((*PT)->Type == OPERATOR) && (ArrayOperators[(*PT)->Value.Index].Class == OP_BR_TWO))
        {
            (*PT)++;
            right_node = GetCommon(PT);
            while ((((*PT)->Type == OPERATOR) && (ArrayOperators[(*PT)->Value.Index].Class != CL_BR_TWO)) || ((*PT)->Type == NUMBER) || ((*PT)->Type == VARIABLE))
            {
                copy_node = right_node;
                right_node = GetCommon(PT);
                if (((*PT)->Type == OPERATOR) && ((ArrayOperators[(*PT)->Value.Index].Class == DIVIDER)||(ArrayOperators[(*PT)->Value.Index].Class == CL_BR_TWO)))
                {
                    right_node = OPR(index_div, copy_node, right_node);
                    (*PT)++;
                }
                else 
                {
                    USER_ERROR(0, ERR_NO_DIVIDER, ArrayOperators[(*PT)->Value.Index].Name, syntax_error = 1)
                    if (syntax_error) 
                    {
                        RecFree(left_node);
                        RecFree(right_node);
                        RecFree(copy_node);
                        return NULL;
                    }
                }
            }
            Token_t* current_token = (*PT);
            current_token++;
            if ((current_token->Type == OPERATOR) && (ArrayOperators[current_token->Value.Index].Class == ELSE))
            {
                (*PT)++;
                int index_else = (*PT)->Value.Index;
                (*PT)++;
                if (((*PT)->Type == OPERATOR) && (ArrayOperators[(*PT)->Value.Index].Class == OP_BR_TWO))
                {
                    (*PT)++;
                    middle_node = GetCommon(PT);
                    while ((((*PT)->Type == OPERATOR) && (ArrayOperators[(*PT)->Value.Index].Class != CL_BR_TWO)) || ((*PT)->Type == NUMBER) || ((*PT)->Type == VARIABLE))
                    {
                        copy_node = middle_node;
                        middle_node = GetCommon(PT);
                        
                        if (((*PT)->Type == OPERATOR)&&((ArrayOperators[(*PT)->Value.Index].Class == DIVIDER)||(ArrayOperators[(*PT)->Value.Index].Class == CL_BR_TWO)))
                        {
                            middle_node = OPR(index_div, middle_node, copy_node);
                            (*PT)++;
                        }
                        else 
                        {
                            USER_ERROR(0, ERR_NO_DIVIDER, ArrayOperators[(*PT)->Value.Index].Name, syntax_error = 1)
                            if (syntax_error) 
                            {
                                RecFree(left_node);
                                RecFree(right_node);
                                RecFree(middle_node);
                                RecFree(copy_node);
                                return NULL;
                            }
                        }
                    }
                    right_node = OPR(index_else, right_node, middle_node);
                }
            }
            return OPR(index_if, left_node, right_node);
        }
        else
        {
            USER_ERROR(0, ERR_NO_OPEN_BRACKET, ArrayOperators[(*PT)->Value.Index].Name, syntax_error = 1)
        }
        return NULL;
    }
    return NULL;
}

static Node_t* GetLoop(Token_t** PtrCurrentToken)
{
    if (syntax_error) 
    {
        return NULL;
    }    
    int index_while = (*PT)->Value.Index;
    Node_t* left_node   = NULL;
    Node_t* right_node  = NULL;
    Node_t* copy_node   = NULL;

    int index_div = 0;
    for (int i = 0; i < (int) SIZE_OF_OPERATORS; i++)
    {
        if (ArrayOperators[i].Class == DIVIDER) {index_div = i; break;}
    }

    if (((*PT)->Type == OPERATOR) && (ArrayOperators[index_while].Class == WHILE))
    {
        (*PT)++;
        if (((*PT)->Type == OPERATOR) && (ArrayOperators[(*PT)->Value.Index].Class == OP_BR_ONE))
        {
            (*PT)++;
            left_node = GetZeroPriority(PT);
        }
        else
        {
            USER_ERROR(0, ERR_NO_OPEN_BRACKET, ArrayOperators[(*PT)->Value.Index].Name, syntax_error = 1)
            return NULL;
        }
        bool check_close_bracket = ((*PT)->Type == OPERATOR) && (ArrayOperators[(*PT)->Value.Index].Class == CL_BR_ONE);
        USER_ERROR(check_close_bracket, ERR_NO_CLOSE_BRACKET, ArrayOperators[(*PT)->Value.Index].Name, syntax_error = 1)
        if (syntax_error) 
        {
            RecFree(left_node);
            return NULL;
        }    
        (*PT)++;
        if (((*PT)->Type == OPERATOR) && (ArrayOperators[(*PT)->Value.Index].Class == OP_BR_TWO))
        {
            (*PT)++;
            right_node = GetCommon(PT);
            while ((((*PT)->Type == OPERATOR) && (ArrayOperators[(*PT)->Value.Index].Class != CL_BR_TWO)) || ((*PT)->Type == NUMBER) || ((*PT)->Type == VARIABLE))
            {
                copy_node = right_node;
                right_node = GetCommon(PT);
                if (((*PT)->Type == OPERATOR)&&((ArrayOperators[(*PT)->Value.Index].Class == DIVIDER)||(ArrayOperators[(*PT)->Value.Index].Class == CL_BR_TWO)))
                {
                    right_node = OPR(index_div, copy_node, right_node);
                    (*PT)++;
                }
                else 
                {
                    USER_ERROR(0, ERR_NO_DIVIDER, ArrayOperators[(*PT)->Value.Index].Name, syntax_error = 1)
                    if (syntax_error) 
                    {
                        RecFree(left_node);
                        RecFree(right_node);
                        RecFree(copy_node);
                        return NULL;
                    }    
                }
            }
            return OPR(index_while, left_node, right_node);
        }
        else
        {
            USER_ERROR(0, ERR_NO_OPEN_BRACKET, ArrayOperators[(*PT)->Value.Index].Name, syntax_error = 1)
        }
        return NULL;
    }
    return NULL;
}

static Node_t* GetFuncCall(Token_t** PtrCurrentToken)
{
    if (syntax_error) 
    {
        return NULL;
    }    
    Token_t* current_token = *PT;
    Node_t*  left_node     = NULL;
    Node_t*  right_node    = NULL;
    Node_t*  copy_node     = NULL;
    if (((current_token)->Type == VARIABLE) && (((++current_token)->Type == OPERATOR) && (ArrayOperators[(current_token)->Value.Index].Class == OP_BR_ONE)))
    {
        int index_call    = 0;
        int index_div_arg = 0;
        for (int i = 0; i < (int) SIZE_OF_OPERATORS; i++)
        {
            if (ArrayOperators[i].Class == CALL) {index_call = i; break;}
        }
        for (int i = 0; i < (int) SIZE_OF_OPERATORS; i++)
        {
            if (ArrayOperators[i].Class == DIVIDER_ARG) {index_div_arg = i; break;}
        }

        left_node = GetVariable(PT);
        if (((*PT)->Type == OPERATOR) && (ArrayOperators[(*PT)->Value.Index].Class != OP_BR_ONE))
        {
            USER_ERROR(0, ERR_NO_OPEN_BRACKET, "_", syntax_error = 1)
            if (syntax_error) 
            {
                RecFree(left_node);
                return NULL;
            }    
        }
        (*PT)++;
        if (((*PT)->Type == OPERATOR) && (ArrayOperators[(*PT)->Value.Index].Class == CL_BR_ONE))
        {
            (*PT)++;
            return OPR(index_call, left_node, NULL);
        }
        //have args
        right_node = GetZeroPriority(PT);
        while (true)
        {
            if (((*PT)->Type == OPERATOR) && (ArrayOperators[(*PT)->Value.Index].Class == CL_BR_ONE))
            {
                break;
            }
            if (((*PT)->Type == OPERATOR) && ((ArrayOperators[(*PT)->Value.Index].Class == DIVIDER_ARG)))
            {
                (*PT)++;
                copy_node = right_node;
                right_node = GetZeroPriority(PT);
                right_node = OPR(index_div_arg, copy_node, right_node);
            }
            else
            {
                USER_ERROR(0, ERR_NO_DIVIDER, ArrayOperators[(*PT)->Value.Index].Name, syntax_error = 1)
                if (syntax_error) 
                {
                    RecFree(left_node);
                    RecFree(right_node);
                    RecFree(copy_node);
                    return NULL;
                }    
            }
        }
        (*PT)++;
        return OPR(index_call, left_node, right_node);
    }
    return NULL;
}

static Node_t* GetFuncDef(Token_t** PtrCurrentToken)
{
    if (syntax_error) 
    {
        return NULL;
    }    
    Node_t*  type_node      = GetType(PT); //start - type
    Token_t* current_token  = *PT; 
    Node_t*  var_node       = NULL; 
    Node_t*  left_node      = NULL;
    Node_t*  right_node     = NULL;
    Node_t*  copy_node      = NULL;

    if ((type_node != NULL) && ((current_token)->Type == VARIABLE) && ((++current_token)->Type == OPERATOR) && (ArrayOperators[(current_token)->Value.Index].Class == OP_BR_ONE))
    {
        int index_div_arg       = 0;
        int index_div           = 0;
        int index_func_def      = 0;
        int index_func_def_help = 0;
        for (int i = 0; i < (int) SIZE_OF_OPERATORS; i++)
        {
            if (ArrayOperators[i].Class == DIVIDER_ARG) {index_div_arg = i; break;}
        }
        for (int i = 0; i < (int) SIZE_OF_OPERATORS; i++)
        {
            if (ArrayOperators[i].Class == DIVIDER) {index_div = i; break;}
        }
        for (int i = 0; i < (int) SIZE_OF_OPERATORS; i++)
        {
            if (ArrayOperators[i].Class == FUNC_DEF) {index_func_def = i; break;}
        }
        for (int i = 0; i < (int) SIZE_OF_OPERATORS; i++)
        {
            if (ArrayOperators[i].Class == FUNC_DEF_HELP) {index_func_def_help = i; break;}
        }

        var_node = GetVariable(PT); //start - variable
        if (((*PT)->Type == OPERATOR)  && (ArrayOperators[(*PT)->Value.Index].Class != OP_BR_ONE))
        {
            USER_ERROR(0, ERR_NO_OPEN_BRACKET, "_", syntax_error = 1)
            if (syntax_error) 
            {
                RecFree(var_node);
                RecFree(type_node);
                return NULL;
            }    
        }
        (*PT)++;
        left_node = GetAssignment(PT); //start - ( ... ) if var(){...} then left node will be NULL
        while (true)
        {
            if (((*PT)->Type == OPERATOR) && (ArrayOperators[(*PT)->Value.Index].Class == CL_BR_ONE))
            {
                break;
            }
            if (((*PT)->Type == OPERATOR) && ((ArrayOperators[(*PT)->Value.Index].Class == DIVIDER_ARG)))
            {
                (*PT)++;
                copy_node = left_node;
                left_node = GetAssignment(PT);
                left_node = OPR(index_div_arg, left_node, copy_node);
            }
            else
            {
                USER_ERROR(0, ERR_NO_DIVIDER, ArrayOperators[(*PT)->Value.Index].Name, syntax_error = 1)
                if(syntax_error)
                {
                    RecFree(left_node);
                    RecFree(var_node);
                    RecFree(type_node);
                    return NULL;
                }   
            }
        }
        (*PT)++;
        //start - { ... }
        if (((*PT)->Type == OPERATOR) && (ArrayOperators[(*PT)->Value.Index].Class == OP_BR_TWO))
        {
            (*PT)++;
            right_node = GetCommon(PT);
            while (((*PT)->Type == OPERATOR) && (ArrayOperators[(*PT)->Value.Index].Class != CL_BR_TWO))
            {
                copy_node = right_node;
                right_node = GetCommon(PT);

                if (((*PT)->Type == OPERATOR) && ((ArrayOperators[(*PT)->Value.Index].Class == DIVIDER)||(ArrayOperators[(*PT)->Value.Index].Class == CL_BR_TWO)))
                {
                    right_node = OPR(index_div, copy_node, right_node);
                    (*PT)++;
                }
                else 
                {
                    USER_ERROR(0, ERR_NO_DIVIDER, ArrayOperators[(*PT)->Value.Index].Name, syntax_error = 1)
                    if (syntax_error) 
                    {
                        RecFree(var_node);
                        RecFree(left_node);
                        RecFree(copy_node);
                        RecFree(type_node);
                        return NULL;
                    }
                }
            }
        }
        else
        {
            USER_ERROR(0, ERR_NO_OPEN_BRACKET, ArrayOperators[(*PT)->Value.Index].Name, syntax_error = 1)
            if (syntax_error) 
            {
                RecFree(var_node);
                RecFree(left_node);
                RecFree(copy_node);
                RecFree(type_node);
                return NULL;
            }    
        }
        return OPR(index_func_def, OPR(index_func_def_help, type_node, var_node), OPR(index_func_def_help, left_node, right_node));
    }
    return NULL;
}

static Node_t* GetType(Token_t** PtrCurrentToken)
{
    if ((*PT)->Type == OPERATOR) 
    {
        EnumOperClass oper_class = ArrayOperators[(*PT)->Value.Index].Class;
        int index_type = (*PT)->Value.Index;
        if ((oper_class == VOID) || (oper_class == VAR))
        {
            (*PT)++;
            return OPR(index_type, NULL, NULL);
        }
    }
    return NULL;
}

//===========================================================================================================

/*
    =====================
    |AUXILIARY FUNCTIONS|
    =====================
*/

static int FindOper(const Token_t* CurrentToken, EnumOperClass CurrentClass)
{
    if ((T->Type == OPERATOR)&&(CurrentClass == ArrayOperators[T->Value.Index].Class))
    {
        return T->Value.Index;
    }
    return NO_FIND;
}

static Node_t* DiffCreateNode (EnumOfType NewType, NodeValue_t NewValue, Node_t* LeftNode, Node_t* RightNode)
{
    Node_t* NewNode = (Node_t*) calloc (1, sizeof (NewNode[0]));
    amount_nodes++;
    MYASSERT(NewNode, ERR_BAD_CALLOC, return NULL)
    InitNode(NewNode);
    NewNode->Left   = LeftNode;
    NewNode->Right  = RightNode;
    NewNode->Type   = NewType;

    if (LeftNode) LeftNode->Parent = NewNode;
    if (RightNode) RightNode->Parent = NewNode;

    if (NewNode->Type == NUMBER) 
    {
        NewNode->Value.Number = NewValue.Number;
    } 
    if ((NewNode->Type == OPERATOR)||(NewNode->Type == VARIABLE))
    {
        NewNode->Value.Index  = NewValue.Index;
    }
    return NewNode;
}

EnumOfErrors CopyVars(BinaryTree_t* myTree, Tokens_t* myTokens)
{
    MYASSERT(myTree, ERR_BAD_POINTER_PASSED_IN_FUNC, return ERR_BAD_POINTER_PASSED_IN_FUNC)
    MYASSERT(myTokens, ERR_BAD_POINTER_PASSED_IN_FUNC, return ERR_BAD_POINTER_PASSED_IN_FUNC)
    MYASSERT(myTree->Variables, ERR_BAD_POINTER_PASSED_IN_FUNC, return ERR_BAD_POINTER_PASSED_IN_FUNC)
    MYASSERT(myTokens->Variables, ERR_BAD_POINTER_PASSED_IN_FUNC, return ERR_BAD_POINTER_PASSED_IN_FUNC)

    for (size_t i = 0; i < SIZE_OF_VARIABLES; i++)
    {
        if (myTokens->Variables[i] != NULL)
        {
            strncpy(myTree->Variables[i].Name, myTokens->Variables[i], SIZE_OF_VAR);
        }
    }
    return ERR_OK;
}
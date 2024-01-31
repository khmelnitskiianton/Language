#include <stdio.h>
#include <string.h>

#include "tree.h"
#include "tokenizer.h"
#include "parser.h"
#include "colors.h"
#include "log.h"
#include "DSL.h"
#include "myassert.h"
#include "verificator.h"

/*
    ========================
    |MAKE RECURSIVE DESCENT|
    ========================
*/

//GetMain not static
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
static bool     InOperators     (Token_t* CurrentToken);
static Node_t*  DiffCreateNode  (EnumOfType NewType, NodeValue_t NewValue, Node_t* LeftNode, Node_t* RightNode);

const int NO_FIND = -1;

Node_t* GetMain(Tokens_t* myTokens)
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
            USER_ERROR(0, ERR_NO_DIVIDER, ArrayOperators[current_token->Value.Index].Name, return NULL)
        }
    }
    return new_node;
}

size_t  CopyVars(BinaryTree_t* myTree, Tokens_t* myTokens)
{
    MYASSERT(myTree, BAD_POINTER_PASSED_IN_FUNC, return 0)
    MYASSERT(myTokens, BAD_POINTER_PASSED_IN_FUNC, return 0)
    MYASSERT(myTree->Variables, BAD_POINTER_PASSED_IN_FUNC, return 0)
    MYASSERT(myTokens->Variables, BAD_POINTER_PASSED_IN_FUNC, return 0)

    for (size_t i = 0; i < SIZE_OF_VARIABLES; i++)
    {
        if (myTokens->Variables[i] != NULL)
        {
            strncpy(myTree->Variables[i], myTokens->Variables[i], SIZE_OF_VAR);
        }
    }
    return 1;
}

static Node_t* GetCommon(Token_t** PtrCurrentToken)
{
    //TODO: get function def
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
    Node_t* new_node = NUM((*PT)->Value.Number);
    (*PT)++;
    return new_node;
}

static Node_t* GetVariable(Token_t** PtrCurrentToken) 
{
    MYASSERT((*PT)->Type == VARIABLE, ERR_BAD_TOKEN, return NULL)
    Node_t* new_node = VAR((*PT)->Value.Index);
    (*PT)++;
    return new_node;
}

static Node_t* GetZeroPriority(Token_t** PtrCurrentToken) 
{
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
    return left_value;
}

static Node_t* GetFirstPriority(Token_t** PtrCurrentToken) 
{
    Node_t* left_value  = GetSecondPriority(PT);
    Node_t* right_value = NULL;

    int index = 0;
    while ((index = FindOper(*PT, FIRST)) != NO_FIND)
    {
        (*PT)++;
        right_value = GetSecondPriority(PT);
        left_value  = OPR(index, left_value, right_value);
    }
    return left_value;
}

static Node_t* GetSecondPriority(Token_t** PtrCurrentToken) 
{
    Node_t* left_value  = GetThirdPriority(PT);
    Node_t* right_value = NULL;

    int index = 0;
    while ((index = FindOper(*PT, SECOND)) != NO_FIND)
    {
        (*PT)++;
        right_value = GetThirdPriority(PT);
        left_value  = OPR(index, left_value, right_value);
    }
    return left_value;
}

static Node_t* GetThirdPriority(Token_t** PtrCurrentToken) 
{
    Node_t* left_value  = GetFourthPriority(PT);
    Node_t* right_value = NULL;

    int index = 0;
    while ((index = FindOper(*PT, THIRD)) != NO_FIND)
    {
        (*PT)++;
        right_value = GetFourthPriority(PT);
        left_value  = OPR(index, left_value, right_value);
    }
    return left_value;
}

static Node_t* GetFourthPriority(Token_t** PtrCurrentToken)
{
    Node_t* left_value  = GetLastPriority(PT);
    Node_t* right_value = NULL;

    int index = 0;
    while ((index = FindOper(*PT, FOURTH)) != NO_FIND)
    {
        (*PT)++;
        right_value = GetLastPriority(PT);
        left_value = OPR(index, left_value, right_value);
    }
    return left_value;
}

static Node_t* GetLastPriority(Token_t** PtrCurrentToken) 
{
    Node_t* val = NULL;
    if (((*PT)->Type == OPERATOR) && (ArrayOperators[(*PT)->Value.Index].Class == OP_BR_ONE))
    {
        (*PT)++;
        val = GetZeroPriority(PT);
        bool check_close_bracket = ((*PT)->Type == OPERATOR) && (ArrayOperators[(*PT)->Value.Index].Class == CL_BR_ONE);
        USER_ERROR(check_close_bracket, ERR_NO_CLOSE_BRACKET, ArrayOperators[(*PT)->Value.Index].Name, return NULL)
        (*PT)++;
        return val;
    }
    if ((*PT)->Type == NUMBER)
    {
        return GetNumber(PT);
    }

    if ((val = GetFuncCall(PT)) != NULL) return val;

    if ((*PT)->Type == VARIABLE)
    {
        return GetVariable(PT);
    }
    return val;
}

static Node_t* GetAssignment(Token_t** PtrCurrentToken)
{
    Token_t* current_token = *PT;
    if (((current_token)->Type == OPERATOR)&&(ArrayOperators[(current_token)->Value.Index].Class == VAR)&&((++current_token)->Type == VARIABLE)&&((++current_token)->Type == OPERATOR)&&(ArrayOperators[(current_token)->Value.Index].Class != OP_BR_ONE))
    {
        int index_var = (*PT)->Value.Index;
        (*PT)++;
        Node_t* right_node = GetVariable(PT);        
        if (((*PT)->Type == OPERATOR) && (ArrayOperators[(*PT)->Value.Index].Class == EQUAL))
        {
            int index_equal = (*PT)->Value.Index;
            (*PT)++;
            Node_t* left_node = GetZeroPriority(PT);
            right_node = OPR(index_equal, right_node, left_node);
        }
        right_node = OPR(index_var, NULL, right_node);
        return right_node;
    }
    else return NULL;
}

static Node_t* GetFuncLoopEnd(Token_t** PtrCurrentToken)
{
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
        if ((InOperators(*PT)) && (ArrayOperators[(*PT)->Value.Index].Class == DIVIDER))
        {
            return OPR(index, left_node, NULL);
        }
        else 
        {
            left_node = GetZeroPriority(PT);
            return OPR(index, left_node, NULL);
        }
    }
    return NULL;
}

static Node_t* GetCondition(Token_t** PtrCurrentToken)
{
    int index_if = (*PT)->Value.Index;
    Node_t* left_node   = NULL;
    Node_t* right_node  = NULL;
    Node_t* copy_node   = NULL;
    Node_t* middle_node = NULL;

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
            USER_ERROR(0, ERR_NO_OPEN_BRACKET, ArrayOperators[(*PT)->Value.Index].Name, return NULL)    
        }
        bool check_close_bracket = ((*PT)->Type == OPERATOR) && (ArrayOperators[(*PT)->Value.Index].Class == CL_BR_ONE);
        USER_ERROR(check_close_bracket, ERR_NO_CLOSE_BRACKET, ArrayOperators[(*PT)->Value.Index].Name, return NULL)
        (*PT)++;
        if (((*PT)->Type == OPERATOR) && (ArrayOperators[(*PT)->Value.Index].Class == OP_BR_TWO))
        {
            (*PT)++;
            right_node = GetCommon(PT);
            while (((*PT)->Type == OPERATOR) && (ArrayOperators[(*PT)->Value.Index].Class != CL_BR_TWO))
            {
                copy_node = right_node;
                right_node = GetCommon(PT);
                if (((*PT)->Type == OPERATOR) && (ArrayOperators[(*PT)->Value.Index].Class == DIVIDER))
                {
                    right_node = OPR((*PT)->Value.Index, copy_node, right_node);
                    (*PT)++;
                }
                else 
                {
                    USER_ERROR(0, ERR_NO_DIVIDER, ArrayOperators[(*PT)->Value.Index].Name, exit(0))
                }
            }
            (*PT)++;
            if (((*PT)->Type == OPERATOR) && (ArrayOperators[(*PT)->Value.Index].Class == ELSE))
            {
                int index_else = (*PT)->Value.Index;
                (*PT)++;
                if (((*PT)->Type == OPERATOR) && (ArrayOperators[(*PT)->Value.Index].Class == OP_BR_TWO))
                {
                    (*PT)++;
                    middle_node = GetCommon(PT);
                    while (((*PT)->Type == OPERATOR) && (ArrayOperators[(*PT)->Value.Index].Class != CL_BR_TWO))
                    {
                        copy_node = middle_node;
                        middle_node = GetCommon(PT);
                        if (((*PT)->Type == OPERATOR)&&(ArrayOperators[(*PT)->Value.Index].Class == DIVIDER))
                        {
                            middle_node = OPR((*PT)->Value.Index, middle_node, copy_node);
                            (*PT)++;
                        }
                        else 
                        {
                            USER_ERROR(0, ERR_NO_DIVIDER, ArrayOperators[(*PT)->Value.Index].Name, return NULL)
                        }
                    }
                    right_node = OPR(index_else, right_node, middle_node);
                    (*PT)++;
                }
            }
            return OPR(index_if, left_node, right_node);
        }
        else
        {
            USER_ERROR(0, ERR_NO_OPEN_BRACKET, ArrayOperators[(*PT)->Value.Index].Name, return NULL)    
        }
        return NULL;
    }
    else return NULL;
}

static Node_t* GetLoop(Token_t** PtrCurrentToken)
{
    int index_while = (*PT)->Value.Index;
    Node_t* left_node   = NULL;
    Node_t* right_node  = NULL;
    Node_t* copy_node   = NULL;

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
            USER_ERROR(0, ERR_NO_OPEN_BRACKET, ArrayOperators[(*PT)->Value.Index].Name, return NULL)    
        }
        bool check_close_bracket = ((*PT)->Type == OPERATOR) && (ArrayOperators[(*PT)->Value.Index].Class == CL_BR_ONE);
        USER_ERROR(check_close_bracket, ERR_NO_CLOSE_BRACKET, ArrayOperators[(*PT)->Value.Index].Name, return NULL)
        (*PT)++;
        if (((*PT)->Type == OPERATOR) && (ArrayOperators[(*PT)->Value.Index].Class == OP_BR_TWO))
        {
            (*PT)++;
            right_node = GetCommon(PT);
            while (((*PT)->Type == OPERATOR) && (ArrayOperators[(*PT)->Value.Index].Class != CL_BR_TWO))
            {
                copy_node = right_node;
                right_node = GetCommon(PT);
                if (((*PT)->Type == OPERATOR)&&(ArrayOperators[(*PT)->Value.Index].Class == DIVIDER))
                {
                    right_node = OPR((*PT)->Value.Index, copy_node, right_node);
                    (*PT)++;
                }
                else 
                {
                    USER_ERROR(0, ERR_NO_DIVIDER, ArrayOperators[(*PT)->Value.Index].Name, return NULL)
                }
            }
            (*PT)++;
            return OPR(index_while, left_node, right_node);
        }
        else
        {
            USER_ERROR(0, ERR_NO_OPEN_BRACKET, ArrayOperators[(*PT)->Value.Index].Name, return NULL)    
        }
        return NULL;
    }
    return NULL;
}

static Node_t* GetFuncCall(Token_t** PtrCurrentToken)
{
    Token_t* current_token = *PT;
    Node_t* left_node  = NULL;
    Node_t* right_node = NULL;
    Node_t* copy_node  = NULL;
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
        if ((InOperators(*PT)) && (ArrayOperators[(*PT)->Value.Index].Class != OP_BR_ONE))
        {
            USER_ERROR(0, ERR_NO_OPEN_BRACKET, "_", return NULL)
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
                USER_ERROR(0, ERR_NO_DIVIDER, ArrayOperators[(*PT)->Value.Index].Name, return NULL)
            }
        }
        (*PT)++;
        return OPR(index_call, left_node, right_node);
    }
    return NULL;
}

static Node_t* GetFuncDef(Token_t** PtrCurrentToken)
{
    Node_t*  type_node      = GetType(PT); //start - type
    Token_t* current_token  = *PT; 
    Node_t*  var_node       = NULL; 
    Node_t*  left_node      = NULL;
    Node_t*  right_node     = NULL;
    Node_t*  copy_node      = NULL;

    if ((type_node != NULL) && ((current_token)->Type == VARIABLE) && ((++current_token)->Type == OPERATOR) && (ArrayOperators[(current_token)->Value.Index].Class == OP_BR_ONE))
    {
        int index_div_arg   = 0;
        int index_func_def      = 0;
        int index_func_def_help = 0;
        for (int i = 0; i < (int) SIZE_OF_OPERATORS; i++)
        {
            if (ArrayOperators[i].Class == DIVIDER_ARG) {index_div_arg = i; break;}
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
        if ((InOperators(*PT)) && (ArrayOperators[(*PT)->Value.Index].Class != OP_BR_ONE))
        {
            USER_ERROR(0, ERR_NO_OPEN_BRACKET, "_", return NULL)
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
                USER_ERROR(0, ERR_NO_DIVIDER, ArrayOperators[(*PT)->Value.Index].Name, return NULL)
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
                if (((*PT)->Type == OPERATOR) && (ArrayOperators[(*PT)->Value.Index].Class == DIVIDER))
                {
                    right_node = OPR((*PT)->Value.Index, copy_node, right_node);
                    (*PT)++;
                }
                else 
                {
                    USER_ERROR(0, ERR_NO_DIVIDER, ArrayOperators[(*PT)->Value.Index].Name, exit(0))
                }
            }
            (*PT)++;
        }
        else
        {
            USER_ERROR(0, ERR_NO_OPEN_BRACKET, ArrayOperators[(*PT)->Value.Index].Name, return NULL)    
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

static int FindOper(const Token_t* CurrentToken, EnumOperClass CurrentClass)
{
    if ((T->Type == OPERATOR)&&(CurrentClass == ArrayOperators[T->Value.Index].Class))
    {
        return T->Value.Index;
    }
    return NO_FIND;
}

static bool InOperators(Token_t* CurrentToken)
{
    return (0 <= (CurrentToken)->Value.Index) && ((CurrentToken)->Value.Index < (int)SIZE_OF_OPERATORS);
}

static Node_t* DiffCreateNode (EnumOfType NewType, NodeValue_t NewValue, Node_t* LeftNode, Node_t* RightNode)
{
    Node_t* NewNode = (Node_t*) calloc (1, sizeof (NewNode[0]));
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
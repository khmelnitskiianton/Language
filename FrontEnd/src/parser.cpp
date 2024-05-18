#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "colors.h"
#include "dsl.h"
#include "myassert.h"

#include "tree.h"
#include "tokenizer.h"
#include "log.h"
#include "parser.h"
#include "verificator.h"

/*
    =================================================
    |MAKE RECURSIVE DESCENT AND WRITING TREE TO FILE|
    =================================================
*/

//=============================================================================================================

static Node_t *GetCommon            (Token_t **PtrToken);
static Node_t *GetNumber            (Token_t **PtrToken);
static Node_t *GetVariable          (Token_t **PtrToken);
static Node_t *GetExpression        (Token_t **PtrToken);
static Node_t *GetFirstPriority     (Token_t **PtrToken);
static Node_t *GetSecondPriority    (Token_t **PtrToken);
static Node_t *GetThirdPriority     (Token_t **PtrToken);
static Node_t *GetFourthPriority    (Token_t **PtrToken);
static Node_t *GetLastPriority      (Token_t **PtrToken);
static Node_t *GetAssignment        (Token_t **PtrToken);
static Node_t *GetContBreakRet      (Token_t **PtrToken);
static Node_t *GetCondition         (Token_t **PtrToken);
static Node_t *GetLoop              (Token_t **PtrToken);
static Node_t *GetFuncCall          (Token_t **PtrToken);
static Node_t *GetFuncDef           (Token_t **PtrToken);
static Node_t *GetType              (Token_t **PtrToken);

static int FindOper(const Token_t *CurrentToken, EnumOperClass CurrentClass);

static const int NO_FIND = -1;
static bool syntax_error = 0;

static BinaryTree_t *ptr_of_tree = NULL;

//=============================================================================================================

/*
    ======================================
    |FUNCTIONS FOR MAKE RECURSIVE DESCENT|
    ======================================
*/

Node_t *GetMain(Tokens_t *myTokens, BinaryTree_t *myTree) {
    Token_t *current_token = myTokens->Data;

    if (current_token->Type == END)
        return NULL; // TREE FOR EMPTY CODE
    
    ptr_of_tree = myTree;
    Node_t *new_node = NULL;
    Node_t *copy_node = NULL;
    
    new_node = GetCommon(&current_token);
    while (current_token->Type != END) {
        copy_node = new_node;
        new_node = GetCommon(&current_token);
        
        if  ((CHECK_TYPE_OPER(current_token)) &&
            ((CHECK_OPER_CLASS(current_token,DIVIDER)) || 
             (CHECK_OPER_CLASS(current_token, CLOSE_BRACKET_FIGURE)))) 
        {
            new_node = OPR(ptr_of_tree, INDEX_SEP, copy_node, new_node);
            current_token++;
        } 
        else 
        {
            USER_ERROR(0, ERR_NO_DIVIDER, ArrayOperators[current_token->Value.Index].Name, syntax_error = 1)
            if (syntax_error) {
                if (!new_node)
                    RecFree(new_node);
                return NULL;
            }
        }
    }
    return new_node;
}

static Node_t *GetCommon(Token_t **PtrToken) {
    if (syntax_error) {
        return NULL;
    }
    Node_t *NewNode = NULL;

    if ((NewNode = GetAssignment(PtrToken))   != NULL)    return NewNode;
    if ((NewNode = GetContBreakRet(PtrToken)) != NULL)    return NewNode;
    if ((NewNode = GetCondition(PtrToken))    != NULL)    return NewNode;
    if ((NewNode = GetLoop(PtrToken))         != NULL)    return NewNode;
    if ((NewNode = GetFuncCall(PtrToken))     != NULL)    return NewNode;
    if ((NewNode = GetFuncDef(PtrToken))      != NULL)    return NewNode;

    //WARNING: don't change 
    if ((NewNode = GetExpression(PtrToken)) != NULL)      return NewNode;

    return NULL;
}

static Node_t *GetNumber(Token_t **PtrToken) {
    MYASSERT(CHECK_TYPE_NUM(*PtrToken), ERR_BAD_TOKEN, return NULL)
    if (syntax_error) {
        return NULL;
    }
    Node_t *new_node = NUM(ptr_of_tree, (*PtrToken)->Value.Number);
    INCREMENT_PTR_TOKEN(PtrToken);
    return new_node;
}

static Node_t *GetVariable(Token_t **PtrToken) {
    MYASSERT(CHECK_TYPE_VAR(*PtrToken), ERR_BAD_TOKEN, return NULL)
    if (syntax_error) {
        return NULL;
    }
    Node_t *new_node = VAR(ptr_of_tree, (*PtrToken)->Value.Index);
    INCREMENT_PTR_TOKEN(PtrToken);
    return new_node;
}

static Node_t *GetExpression(Token_t **PtrToken) {
    if (syntax_error) {
        return NULL;
    }
    
    Node_t *left_value = GetFirstPriority(PtrToken);
    
    if (left_value == NULL)
        return NULL;
    Node_t *right_value = NULL;

    int index = 0;
    while ((index = FindOper(*PtrToken, ZERO)) != NO_FIND) {
        INCREMENT_PTR_TOKEN(PtrToken);
        right_value = GetFirstPriority(PtrToken);
        left_value = OPR(ptr_of_tree, index, left_value, right_value);
    }

    if (syntax_error) {
        RecFree(left_value);
        return NULL;
    }

    return left_value;
}

static Node_t *GetFirstPriority(Token_t **PtrToken) {
    if (syntax_error) {
        return NULL;
    }
    Node_t *left_value = GetSecondPriority(PtrToken);
    Node_t *right_value = NULL;

    int index = 0;
    while ((index = FindOper(*PtrToken, FIRST)) != NO_FIND) {
        INCREMENT_PTR_TOKEN(PtrToken);
        right_value = GetSecondPriority(PtrToken);
        left_value = OPR(ptr_of_tree, index, left_value, right_value);
    }

    if (syntax_error) {
        RecFree(left_value);
        return NULL;
    }

    return left_value;
}

static Node_t *GetSecondPriority(Token_t **PtrToken) {
    if (syntax_error) {
        return NULL;
    }

    Node_t *left_value = GetThirdPriority(PtrToken);
    Node_t *right_value = NULL;

    int index = 0;
    while ((index = FindOper(*PtrToken, SECOND)) != NO_FIND) {
        INCREMENT_PTR_TOKEN(PtrToken);
        right_value = GetThirdPriority(PtrToken);
        left_value = OPR(ptr_of_tree, index, left_value, right_value);
    }

    if (syntax_error) {
        RecFree(left_value);
        return NULL;
    }
    return left_value;
}

static Node_t *GetThirdPriority(Token_t **PtrToken) {
    if (syntax_error) {
        return NULL;
    }

    Node_t *left_value = GetFourthPriority(PtrToken);
    Node_t *right_value = NULL;

    int index = 0;
    while ((index = FindOper(*PtrToken, THIRD)) != NO_FIND) {
        INCREMENT_PTR_TOKEN(PtrToken);
        right_value = GetFourthPriority(PtrToken);
        left_value = OPR(ptr_of_tree, index, left_value, right_value);
    }

    if (syntax_error) {
        RecFree(left_value);
        return NULL;
    }
    return left_value;
}

static Node_t *GetFourthPriority(Token_t **PtrToken) {
    if (syntax_error) {
        return NULL;
    }
    Node_t *left_value = GetLastPriority(PtrToken);
    Node_t *right_value = NULL;

    int index = 0;
    while ((index = FindOper(*PtrToken, FOURTH)) != NO_FIND) {
        INCREMENT_PTR_TOKEN(PtrToken);
        right_value = GetLastPriority(PtrToken);
        left_value = OPR(ptr_of_tree, index, left_value, right_value);
    }

    if (syntax_error) {
        RecFree(left_value);
        return NULL;
    }
    return left_value;
}

static Node_t *GetLastPriority(Token_t **PtrToken) {
    if (syntax_error) {
        return NULL;
    }

    Node_t *val = NULL;
    
    if  ((CHECK_TYPE_OPER(*PtrToken)) &&
         (CHECK_OPER_CLASS(*PtrToken,OPEN_BRACKET_ROUND)))
    {
        INCREMENT_PTR_TOKEN(PtrToken);

        val = GetExpression(PtrToken);
        
        bool check_close_bracket =
             (CHECK_TYPE_OPER(*PtrToken)) &&
             (CHECK_OPER_CLASS(*PtrToken,CLOSE_BRACKET_ROUND));
        
        USER_ERROR(check_close_bracket, ERR_NO_CLOSE_BRACKET, ArrayOperators[(*PtrToken)->Value.Index].Name, syntax_error = 1)
        
        if (syntax_error) {
            RecFree(val);
            return NULL;
        }
        INCREMENT_PTR_TOKEN(PtrToken);
        return val;
    }
    if (CHECK_TYPE_NUM(*PtrToken)) {
        return GetNumber(PtrToken);
    }

    if ((val = GetFuncCall(PtrToken)) != NULL) {
        if (syntax_error) {
            RecFree(val);
            return NULL;
        }
        return val;
    }

    if (syntax_error) {
        RecFree(val);
        return NULL;
    }

    if (CHECK_TYPE_VAR(*PtrToken)) {
        return GetVariable(PtrToken);
    }

    if (syntax_error) {
        RecFree(val);
        return NULL;
    }

    return val;
}

static Node_t *GetAssignment(Token_t **PtrToken) {
    if (syntax_error) {
        return NULL;
    }

    Token_t *current_token = *PtrToken;

    //Check for assignment
    if (!(CHECK_TYPE_OPER(current_token)))                      return NULL;
    if (!(CHECK_OPER_CLASS(current_token, VAR)))                return NULL;
    if (!(CHECK_TYPE_VAR(++current_token)))                     return NULL;
    if (!(CHECK_TYPE_OPER(++current_token)))                    return NULL;
    if ((CHECK_OPER_CLASS(current_token, OPEN_BRACKET_ROUND)))  return NULL; 

    INCREMENT_PTR_TOKEN(PtrToken);

    Node_t *left_node   = GetVariable(PtrToken);
    Node_t *right_node  = NULL;

    if ((CHECK_TYPE_OPER(*PtrToken)) &&
        (CHECK_OPER_CLASS(*PtrToken, EQUAL))) 
    {
        int index_equal = (*PtrToken)->Value.Index;
        INCREMENT_PTR_TOKEN(PtrToken);

        right_node = GetExpression(PtrToken);
        right_node = OPR(ptr_of_tree, index_equal, left_node, right_node);
        
        if (syntax_error) {
            RecFree(right_node);
            return NULL;
        }

    } 
    else {
        right_node = left_node;
    }

    Node_t* new_node = OPR(ptr_of_tree, INDEX_VAR, NULL, right_node);
    
    if (syntax_error) {
        RecFree(new_node);
        return NULL;
    }

    return new_node;
}

static Node_t *GetContBreakRet(Token_t **PtrToken) {
    if (syntax_error) {
        return NULL;
    }

    if ((CHECK_TYPE_OPER(*PtrToken)) && 
        (CHECK_OPER_CLASS(*PtrToken, BREAK))) 
    {
        INCREMENT_PTR_TOKEN(PtrToken);
        return OPR(ptr_of_tree, INDEX_BREAK, NULL, NULL);
    }
    
    if ((CHECK_TYPE_OPER(*PtrToken)) && 
        (CHECK_OPER_CLASS(*PtrToken, CONTINUE))) 
    {
        INCREMENT_PTR_TOKEN(PtrToken);
        return OPR(ptr_of_tree, INDEX_CONTINUE, NULL, NULL);
    }

    if ((CHECK_TYPE_OPER(*PtrToken)) && 
        (CHECK_OPER_CLASS(*PtrToken, RETURN))) 
    {
        INCREMENT_PTR_TOKEN(PtrToken);
        Node_t *left_node = NULL;

        if ((CHECK_TYPE_OPER(*PtrToken)) &&
            (CHECK_OPER_CLASS(*PtrToken, DIVIDER))) {
            return OPR(ptr_of_tree, INDEX_RETURN, NULL, NULL);
        } 
        else 
        {
            left_node = GetExpression(PtrToken);
            return OPR(ptr_of_tree, INDEX_RETURN, left_node, NULL);
        }
    }
    return NULL;
}

static Node_t *GetCondition(Token_t **PtrToken) {
    if (syntax_error) {
        return NULL;
    }

    //check for if
    if (!(CHECK_TYPE_OPER(*PtrToken)))      return NULL;
    if (!(CHECK_OPER_CLASS(*PtrToken, IF))) return NULL; 
    INCREMENT_PTR_TOKEN(PtrToken);

    Node_t* left_node = NULL;
    
    if ((CHECK_TYPE_OPER(*PtrToken)) &&
        (CHECK_OPER_CLASS(*PtrToken, OPEN_BRACKET_ROUND))) 
    {
        INCREMENT_PTR_TOKEN(PtrToken);
        left_node = GetExpression(PtrToken);
    } 
    else 
    {
        USER_ERROR(0, ERR_NO_OPEN_BRACKET, ArrayOperators[(*PtrToken)->Value.Index].Name, syntax_error = 1)
        return NULL;
    }

    bool check_close_bracket =  (CHECK_TYPE_OPER(*PtrToken)) &&
                                (CHECK_OPER_CLASS(*PtrToken, CLOSE_BRACKET_ROUND));
    USER_ERROR(check_close_bracket, ERR_NO_CLOSE_BRACKET, ArrayOperators[(*PtrToken)->Value.Index].Name, syntax_error = 1)
    INCREMENT_PTR_TOKEN(PtrToken);

    if (syntax_error) {
        RecFree(left_node);
        return NULL;
    }

    if ((CHECK_TYPE_OPER(*PtrToken)) &&
        (CHECK_OPER_CLASS(*PtrToken, OPEN_BRACKET_FIGURE))) 
    {
        INCREMENT_PTR_TOKEN(PtrToken);
    } 
    else 
    {
        USER_ERROR(0, ERR_NO_OPEN_BRACKET, ArrayOperators[(*PtrToken)->Value.Index].Name, syntax_error = 1)
        return NULL;
    }

    Node_t *right_node  = NULL;
    Node_t *copy_node   = NULL;

    //{} main if
    while (((CHECK_TYPE_OPER(*PtrToken)) && !(CHECK_OPER_CLASS(*PtrToken, CLOSE_BRACKET_FIGURE))) ||
            (CHECK_TYPE_NUM(*PtrToken)) || 
            (CHECK_TYPE_VAR(*PtrToken))) 
    {
        copy_node = right_node;
        right_node = GetCommon(PtrToken);
        if ((CHECK_TYPE_OPER(*PtrToken)) &&
            ((CHECK_OPER_CLASS(*PtrToken, DIVIDER)) ||
             (CHECK_OPER_CLASS(*PtrToken, CLOSE_BRACKET_FIGURE)))) 
        {
            right_node = OPR(ptr_of_tree, INDEX_SEP, copy_node, right_node);
        } 
        else 
        {
            USER_ERROR(0, ERR_NO_DIVIDER, ArrayOperators[(*PtrToken)->Value.Index].Name, syntax_error = 1)
            if (syntax_error) {
                RecFree(left_node);
                RecFree(right_node);
                RecFree(copy_node);
                return NULL;
            }
        }
        INCREMENT_PTR_TOKEN(PtrToken);
    }
    //ELSE    
    Token_t *current_token = (*PtrToken);
    current_token++;
    if ((CHECK_OPER_CLASS(*PtrToken, DIVIDER)) &&
        (CHECK_OPER_CLASS(current_token, ELSE)))
    {
        INCREMENT_PTR_TOKEN(PtrToken);
        INCREMENT_PTR_TOKEN(PtrToken);
    }
    else 
    {
        return OPR(ptr_of_tree, INDEX_IF, left_node, right_node);
    }

    if ((CHECK_TYPE_OPER(*PtrToken)) &&
        (CHECK_OPER_CLASS(*PtrToken, OPEN_BRACKET_FIGURE))) 
    {
        INCREMENT_PTR_TOKEN(PtrToken);

        Node_t* middle_node = NULL;
        while (((CHECK_TYPE_OPER(*PtrToken)) && !(CHECK_OPER_CLASS(*PtrToken, CLOSE_BRACKET_FIGURE))) ||
               (CHECK_TYPE_NUM(*PtrToken)) ||
               (CHECK_TYPE_VAR(*PtrToken))) 
        {
            copy_node = middle_node;
            middle_node = GetCommon(PtrToken);
                
            if ((CHECK_TYPE_OPER(*PtrToken)) &&
                ((CHECK_OPER_CLASS(*PtrToken, DIVIDER)) ||
                 (CHECK_OPER_CLASS(*PtrToken, CLOSE_BRACKET_FIGURE)))) 
            {
                middle_node = OPR(ptr_of_tree, INDEX_SEP, middle_node, copy_node);
            } 
            else 
            {
                USER_ERROR(0, ERR_NO_DIVIDER, ArrayOperators[(*PtrToken)->Value.Index].Name, syntax_error = 1)
                if (syntax_error) {
                    RecFree(left_node);
                    RecFree(right_node);
                    RecFree(middle_node);
                    RecFree(copy_node);
                    return NULL;
                }
            }
            INCREMENT_PTR_TOKEN(PtrToken);
        }
        right_node = OPR(ptr_of_tree, INDEX_ELSE, right_node, middle_node);
    }
    return OPR(ptr_of_tree, INDEX_IF, left_node, right_node);
}

static Node_t *GetLoop(Token_t **PtrToken) {
   if (syntax_error) {
        return NULL;
    }

    //check for if
    if (!(CHECK_TYPE_OPER(*PtrToken)))      return NULL;
    if (!(CHECK_OPER_CLASS(*PtrToken, WHILE))) return NULL; 
    INCREMENT_PTR_TOKEN(PtrToken);

    Node_t* left_node = NULL;
    
    if ((CHECK_TYPE_OPER(*PtrToken)) &&
        (CHECK_OPER_CLASS(*PtrToken, OPEN_BRACKET_ROUND))) 
    {
        INCREMENT_PTR_TOKEN(PtrToken);
        left_node = GetExpression(PtrToken);
    } 
    else 
    {
        USER_ERROR(0, ERR_NO_OPEN_BRACKET,ArrayOperators[(*PtrToken)->Value.Index].Name, syntax_error = 1)
        return NULL;
    }

    bool check_close_bracket =  (CHECK_TYPE_OPER(*PtrToken)) &&
                                (CHECK_OPER_CLASS(*PtrToken, CLOSE_BRACKET_ROUND));
    USER_ERROR(check_close_bracket, ERR_NO_CLOSE_BRACKET, ArrayOperators[(*PtrToken)->Value.Index].Name, syntax_error = 1)
    INCREMENT_PTR_TOKEN(PtrToken);

    if (syntax_error) {
        RecFree(left_node);
        return NULL;
    }
    if ((CHECK_TYPE_OPER(*PtrToken)) &&
        (CHECK_OPER_CLASS(*PtrToken, OPEN_BRACKET_FIGURE))) 
    {
        INCREMENT_PTR_TOKEN(PtrToken);
    } 
    else 
    {
        USER_ERROR(0, ERR_NO_OPEN_BRACKET, ArrayOperators[(*PtrToken)->Value.Index].Name, syntax_error = 1)
        return NULL;
    }

    Node_t *right_node  = NULL;
    Node_t *copy_node   = NULL;

    //{} main if
    while (((CHECK_TYPE_OPER(*PtrToken)) && !(CHECK_OPER_CLASS(*PtrToken, CLOSE_BRACKET_FIGURE))) ||
            (CHECK_TYPE_NUM(*PtrToken)) || 
            (CHECK_TYPE_VAR(*PtrToken))) 
    {
        copy_node = right_node;
        right_node = GetCommon(PtrToken);
        if ((CHECK_TYPE_OPER(*PtrToken)) &&
            ((CHECK_OPER_CLASS(*PtrToken, DIVIDER)) ||
             (CHECK_OPER_CLASS(*PtrToken, CLOSE_BRACKET_FIGURE)))) 
        {
            right_node = OPR(ptr_of_tree, INDEX_SEP, copy_node, right_node);
        } 
        else 
        {
            USER_ERROR(0, ERR_NO_DIVIDER, ArrayOperators[(*PtrToken)->Value.Index].Name, syntax_error = 1)
            if (syntax_error) {
                RecFree(left_node);
                RecFree(right_node);
                RecFree(copy_node);
                return NULL;
            }
        }
        INCREMENT_PTR_TOKEN(PtrToken);
    }
    return OPR(ptr_of_tree, INDEX_WHILE, left_node, right_node);
}

static Node_t *GetFuncCall(Token_t **PtrToken) {
    if (syntax_error) {
        return NULL;
    }
    Token_t *current_token = *PtrToken;
    Node_t *left_node = NULL;
    Node_t *right_node = NULL;
    Node_t *copy_node = NULL;

    if  (!(CHECK_TYPE_VAR(current_token)))                      return NULL;
    if  (!(CHECK_TYPE_OPER(++current_token)))                   return NULL;
    if  (!(CHECK_OPER_CLASS(current_token, OPEN_BRACKET_ROUND)))    return NULL;

    left_node = GetVariable(PtrToken);
    
    if ((CHECK_TYPE_OPER(*PtrToken)) &&
        (!(CHECK_OPER_CLASS(*PtrToken, OPEN_BRACKET_ROUND)))) 
    {
        USER_ERROR(0, ERR_NO_OPEN_BRACKET, "_", syntax_error = 1)
        if (syntax_error) {
            RecFree(left_node);
            return NULL;
        }
    }

    INCREMENT_PTR_TOKEN(PtrToken);

    if ((CHECK_TYPE_OPER(*PtrToken)) &&
        (CHECK_OPER_CLASS(*PtrToken, CLOSE_BRACKET_ROUND))) {
        INCREMENT_PTR_TOKEN(PtrToken);
        return OPR(ptr_of_tree, INDEX_CALL, left_node, NULL);
    }
    
    // have args
    right_node = GetExpression(PtrToken);
    while (true) {
        if ((CHECK_TYPE_OPER(*PtrToken)) &&
            (CHECK_OPER_CLASS(*PtrToken, CLOSE_BRACKET_ROUND)))
        {
            break;
        }
        if ((CHECK_TYPE_OPER(*PtrToken)) &&
            ((CHECK_OPER_CLASS(*PtrToken, DIVIDER_ARG))))
        {
            INCREMENT_PTR_TOKEN(PtrToken);
            copy_node  = right_node;
            right_node = GetExpression(PtrToken);
            right_node = OPR(ptr_of_tree, INDEX_SEP_ARG, copy_node, right_node);
        } 
        else 
        {
            USER_ERROR(0, ERR_NO_DIVIDER, ArrayOperators[(*PtrToken)->Value.Index].Name, syntax_error = 1)
            if (syntax_error) {
                RecFree(left_node);
                RecFree(right_node);
                RecFree(copy_node);
                return NULL;
            }
        }
    }
    INCREMENT_PTR_TOKEN(PtrToken);
    return OPR(ptr_of_tree, INDEX_CALL, left_node, right_node);
}

static Node_t *GetFuncDef(Token_t **PtrToken) {
    if (syntax_error) {
        return NULL;
    }
    Node_t *type_node = GetType(PtrToken); // start - type
    Token_t *current_token = *PtrToken;

    if  (!(type_node != NULL) || !(CHECK_TYPE_VAR(current_token)))  return NULL;
    if  (!(CHECK_TYPE_OPER(++current_token)))                       return NULL;
    if  (!(CHECK_OPER_CLASS(current_token, OPEN_BRACKET_ROUND)))    return NULL; 
 
    Node_t *var_node = NULL;
    Node_t *left_node = NULL;

    var_node = GetVariable(PtrToken); // start - variable
    INCREMENT_PTR_TOKEN(PtrToken);
    left_node = GetAssignment(PtrToken);    // start - ( ... ) if VAR(ptr_of_tree,
                                            // ){...} then left node will be NULL

    Node_t *right_node = NULL;
    Node_t *copy_node = NULL;
    while (true) {
        if ((CHECK_TYPE_OPER(*PtrToken)) &&
            (CHECK_OPER_CLASS(*PtrToken, CLOSE_BRACKET_ROUND))) 
        {
            break;
        }
        if ((CHECK_TYPE_OPER(*PtrToken)) &&
            ((CHECK_OPER_CLASS(*PtrToken, DIVIDER_ARG)))) 
        {
            INCREMENT_PTR_TOKEN(PtrToken);
            copy_node = left_node;
            left_node = GetAssignment(PtrToken);
            left_node = OPR(ptr_of_tree, INDEX_SEP_ARG, copy_node, left_node);
        } 
        else 
        {
            USER_ERROR(0, ERR_NO_DIVIDER, ArrayOperators[(*PtrToken)->Value.Index].Name, syntax_error = 1)
            if (syntax_error) {
                RecFree(left_node);
                RecFree(var_node);
                RecFree(type_node);
                return NULL;
            }
        }
    }
    INCREMENT_PTR_TOKEN(PtrToken);
    // start - { ... }
    if ((CHECK_TYPE_OPER(*PtrToken)) &&
        (CHECK_OPER_CLASS(*PtrToken, OPEN_BRACKET_FIGURE))) 
    {
        INCREMENT_PTR_TOKEN(PtrToken);
    }
    else
    {
        USER_ERROR(0, ERR_NO_OPEN_BRACKET, ArrayOperators[(*PtrToken)->Value.Index].Name, syntax_error = 1)
        if (syntax_error) {
            RecFree(var_node);
            RecFree(left_node);
            RecFree(copy_node);
            RecFree(type_node);
            return NULL;
        }
    }

    right_node = NULL;
    while (((CHECK_TYPE_OPER(*PtrToken)) && !(CHECK_OPER_CLASS(*PtrToken, CLOSE_BRACKET_FIGURE))) ||
           (CHECK_TYPE_NUM(*PtrToken)) || 
           (CHECK_TYPE_VAR(*PtrToken))) 
    {
        copy_node = right_node;
        right_node = GetCommon(PtrToken);
        if ((CHECK_TYPE_OPER(*PtrToken)) &&
            ((CHECK_OPER_CLASS(*PtrToken, DIVIDER)) ||
             (CHECK_OPER_CLASS(*PtrToken, CLOSE_BRACKET_FIGURE)))) 
        {
            right_node = OPR(ptr_of_tree, INDEX_SEP, copy_node, right_node);
        } 
        else
        {
            USER_ERROR(0, ERR_NO_DIVIDER, ArrayOperators[(*PtrToken)->Value.Index].Name, syntax_error = 1)
            if (syntax_error) {
                RecFree(var_node);
                RecFree(left_node);
                RecFree(copy_node);
                RecFree(type_node);
                return NULL;
            }
        }
        INCREMENT_PTR_TOKEN(PtrToken);
    }
    return OPR(ptr_of_tree, INDEX_FUNC_DEF, OPR(ptr_of_tree, INDEX_FUNC_HEL, type_node, var_node), OPR(ptr_of_tree, INDEX_FUNC_HEL, left_node, right_node));
}

static Node_t *GetType(Token_t **PtrToken) {
    if (CHECK_TYPE_OPER(*PtrToken)) {
        EnumOperClass oper_class = ArrayOperators[(*PtrToken)->Value.Index].Class;
        int index_type = (*PtrToken)->Value.Index;
        if ((oper_class == VOID) || (oper_class == VAR)) {
            INCREMENT_PTR_TOKEN(PtrToken);
            return OPR(ptr_of_tree, index_type, NULL, NULL);
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

static int FindOper(const Token_t *CurrentToken, EnumOperClass CurrentClass) {
    if ((CurrentToken->Type == OPERATOR) &&
        (CurrentClass == ArrayOperators[CurrentToken->Value.Index].Class)) {
        return CurrentToken->Value.Index;
    }
    return NO_FIND;
}

EnumOfErrors CopyVars(BinaryTree_t *myTree, Tokens_t *myTokens) {
    MYASSERT(myTree, ERR_BAD_POINTER_PASSED_IN_FUNC,
             return ERR_BAD_POINTER_PASSED_IN_FUNC)
    MYASSERT(myTokens, ERR_BAD_POINTER_PASSED_IN_FUNC,
             return ERR_BAD_POINTER_PASSED_IN_FUNC)
    MYASSERT(myTree->Variables, ERR_BAD_POINTER_PASSED_IN_FUNC,
             return ERR_BAD_POINTER_PASSED_IN_FUNC)
    MYASSERT(myTokens->Variables, ERR_BAD_POINTER_PASSED_IN_FUNC,
             return ERR_BAD_POINTER_PASSED_IN_FUNC)

    for (size_t i = 0; i < SIZE_OF_VARIABLES; i++) {
        if (myTokens->Variables[i] != NULL) {
            strncpy(myTree->Variables[i].Name, myTokens->Variables[i],
                    SIZE_OF_VAR);
        }
    }
    return ERR_OK;
}
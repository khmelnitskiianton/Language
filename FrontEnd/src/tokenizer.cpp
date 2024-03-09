#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "colors.h"
#include "tree.h"
#include "tokenizer.h"
#include "parser.h"
#include "log.h"
#include "myassert.h"
#include "verificator.h"

/*
    ======================
    |MAKE ARRAY OF TOKENS|
    ======================
*/

//=============================================================================================================

static const int    NOT_IN_OPER   = -1;
static const size_t MULTIPLIER    = 2;
static const size_t INIT_CAPACITY = 10;

static FILE*        OpenFile            (const char* file_open, const char* option);
static void         CloseFile           (FILE* file_text);
static size_t       FileSize            (FILE *file_text);
static void         InitToken           (Token_t* Token);
static int          MemPoison           (void* memptr, size_t num);
static Token_t*     Expand              (Tokens_t *TokensArr);
static int          StrInOperators      (const char* str);
static char*        SkipSpaces          (char* current_ptr, char* end_ptr);
static void         SkipComments        (char* buffer, char* end_ptr);
static void         PutNull             (char* start_ptr, char* end_ptr);
static bool         CompareVar          (const char* str1, const char* str2, size_t num);
static char*        ProcessVariable     (Tokens_t* TokensArr, char* current_ptr);
static int          InsertVariable      (Tokens_t* TokensArr, char* current_ptr);
static char*        ProcessNumber       (Tokens_t* TokensArr, char* current_ptr, bool* token_continue);
static char*        ProcessOperator     (Tokens_t* TokensArr, char* current_ptr, bool* token_continue);
static EnumOperType TypeOfChar          (char unknown_char);
static size_t       TokenSize           (const char* str);
static bool         CheckIfVar          (const char* maybe_var);

//========================================================================================================
//READ TEXT FROM FILE

char* CreateDirtyBuffer (const char* file_database)
{
    MYASSERT(file_database, ERR_WHAT_FILE_OF_DATA, return NULL)

    FILE* FileRead = OpenFile (file_database, "rb");
    if (!FileRead) return NULL;

    size_t size_text = FileSize (FileRead);
    char* text_buffer = NULL;
    text_buffer = (char*) calloc (size_text + 1, sizeof (char));
    MYASSERT(text_buffer, ERR_BAD_CALLOC, return NULL)
    size_t result_size = fread (text_buffer, 1, size_text, FileRead);
    MYASSERT(result_size == size_text, ERR_BAD_FREAD, return NULL)

	*(text_buffer + size_text) = '\0';
    CloseFile (FileRead);

    return text_buffer;   
}

static FILE* OpenFile (const char* file_open, const char* option)
{
    FILE *FileOpen = fopen (file_open, option);
    MYASSERT(FileOpen, ERR_OPEN_FILE, return NULL)
    return FileOpen;
}

static void CloseFile (FILE* file_text)
{
	MYASSERT(file_text, BAD_POINTER_PASSED_IN_FUNC, assert(0))
    int result = fclose(file_text);
	MYASSERT(!result, ERR_CLOSE_FILE, assert(0))
}

static size_t FileSize (FILE *file_text)
{
    MYASSERT(file_text, ERR_BAD_POINTER_FILE, return 0)
	struct stat st;
    int fd = fileno (file_text); 
    fstat (fd, &st);
    size_t size_text = (size_t) st.st_size;
	return size_text;
}

//========================================================================================================
//CREATE ARRAY OF TOKENS

void TokensCtor (Tokens_t *TokensArr)
{
    MYASSERT(TokensArr, ERR_BAD_POINTER_PASSED_IN_FUNC, return)
    TokensArr->Capacity = 1;
    TokensArr->Data = (Token_t*) calloc (1, sizeof(Token_t));
    TokensArr->Size = 0;
}

void TokensDtor (Tokens_t *TokensArr)
{
    MYASSERT(TokensArr, ERR_BAD_POINTER_PASSED_IN_FUNC, return)
    free(TokensArr->Data);
}

EnumOfErrors CreateTokens(Tokens_t* TokensArr, char* dirty_buffer)
{
    MYASSERT(TokensArr, ERR_BAD_POINTER_PASSED_IN_FUNC, return ERR_BAD_POINTER_PASSED_IN_FUNC)
    bool token_continue = 0;
    char* current_ptr = dirty_buffer;
    size_t buffer_size = 0;
    while (*(current_ptr + buffer_size) != '\0')
    {
        buffer_size++;
    }
    //CLEAN COMMENTS
    SkipComments(current_ptr, dirty_buffer + buffer_size);
    PutNull(current_ptr, current_ptr + buffer_size);
    while (current_ptr < (dirty_buffer + buffer_size))
    {
        //SKIP ZERO AND SPACES
        current_ptr = SkipSpaces(current_ptr, dirty_buffer + buffer_size);
        //PROCESS NUMBER
        current_ptr = ProcessNumber(TokensArr, current_ptr, &token_continue);
        //PROCESS OPERATOR
        current_ptr = ProcessOperator(TokensArr, current_ptr, &token_continue);
        //PROCESS VARIABLE
        if (!token_continue) 
        {
            if (current_ptr == NULL) {break;}
            USER_ERROR(CheckIfVar(current_ptr), ERR_UNKNOWN_OBJ, current_ptr, exit(0))
            current_ptr = ProcessVariable(TokensArr, current_ptr);
        }
        token_continue = 0;
    }
    //PUT 2 END TOKEN
    if (TokensArr->Size == (int) TokensArr->Capacity)
    {
        TokensArr->Data = Expand(TokensArr);
    }
    (TokensArr->Data + TokensArr->Size)->Type        = END;
    (TokensArr->Data + TokensArr->Size)->Value.Index = 0;
    (TokensArr->Size)++;
    return ERR_OK;
}

static bool CheckIfVar(const char* maybe_var)
{
    if (isalnum(*maybe_var) || (*maybe_var == '_')) return true;
    return false;    
}

static char* ProcessVariable(Tokens_t* TokensArr, char* current_ptr)
{
    char* new_ptr = current_ptr;
    if (new_ptr == NULL) return new_ptr;
    int index = InsertVariable (TokensArr, current_ptr);
    if (TokensArr->Size == (int) TokensArr->Capacity) //expansion
    {
        TokensArr->Data = Expand(TokensArr);
    }
    (TokensArr->Data + TokensArr->Size)->Type        = VARIABLE;
    (TokensArr->Data + TokensArr->Size)->Value.Index = index;
    (TokensArr->Size)++;
    USER_ERROR(index != -1, ERR_OVERFLOW_VARIABLES, "", exit(0))
    size_t size = strlen(TokensArr->Variables[index]);
    new_ptr = new_ptr + size;
    return new_ptr;
}

static int InsertVariable (Tokens_t* TokensArr, char* current_ptr)
{
    for (size_t i = 0; i < SIZE_OF_VARIABLES; i++)
    {
        if (CompareVar(TokensArr->Variables[i], current_ptr, SIZE_OF_VAR))
        {
            return (int) i;
        }
        if (!(*(TokensArr->Variables[i]))) //finding free place in array of variables
        {
            //copy!
            size_t j = 0;
            while (isalnum(*(current_ptr + j)) || (*(current_ptr + j) == '_'))
            {
                TokensArr->Variables[i][j] = *(current_ptr + j);
                j++;
            }
            TokensArr->Variables[i][j] = '\0';
            return (int) i;
        }
    }
    return -1;
}

static char* ProcessNumber(Tokens_t* TokensArr, char* current_ptr, bool* token_continue)
{
    char* new_ptr = current_ptr;
    if (new_ptr == NULL) return new_ptr;
    double number = strtod(current_ptr, &new_ptr);
    if (isdigit(*current_ptr)) //creating token with number
    {
        if (TokensArr->Size == (int) TokensArr->Capacity) //expansion
        {
            TokensArr->Data = Expand(TokensArr);
        }
        (TokensArr->Data + TokensArr->Size)->Type         = NUMBER;
        (TokensArr->Data + TokensArr->Size)->Value.Number = number;
        (TokensArr->Size)++;
        *token_continue = 1;
    }
    else 
    {
        return current_ptr;
    }
    return new_ptr;
}

static char* ProcessOperator(Tokens_t* TokensArr, char* current_ptr, bool* token_continue)
{
    char* new_ptr = current_ptr;
    if (new_ptr == NULL) return new_ptr;
    int index = StrInOperators(current_ptr);
    if (index != NOT_IN_OPER) //creating token with operator
    {
        if (TokensArr->Size == (int) TokensArr->Capacity) //expansion
        {
            TokensArr->Data = Expand(TokensArr);
        }
        (TokensArr->Data + TokensArr->Size)->Type        = OPERATOR;
        (TokensArr->Data + TokensArr->Size)->Value.Index = index;
        (TokensArr->Size)++;
        *token_continue = 1;

        //change pointer
        size_t size = strlen(ArrayOperators[index].Name);
        new_ptr = new_ptr + size;
    }
    return new_ptr;
}

static Token_t* Expand (Tokens_t *TokensArr)
{
    MYASSERT(TokensArr, ERR_BAD_POINTER_PASSED_IN_FUNC, return NULL)
    void* new_place = nullptr;
    TokensArr->Capacity = (TokensArr->Capacity) * MULTIPLIER;                                                                                      
    new_place = realloc((TokensArr->Data), sizeof(Token_t)*(TokensArr->Capacity));
    MYASSERT(new_place, ERR_BAD_REALLOC, return NULL)
                                                 
    MemPoison(new_place + sizeof(Token_t)*((size_t)(TokensArr->Size)), (TokensArr->Capacity)-((size_t)(TokensArr->Size)));                        

    return (Token_t*) new_place;
}

static int MemPoison(void* memptr, size_t num)
{
    for (size_t i = 0; i < num; i++)
    {
        InitToken(((Token_t*) memptr) + i);
    }
    return 1;
}

static void InitToken(Token_t* Token)
{
    MYASSERT(Token, ERR_BAD_POINTER_PASSED_IN_FUNC, return)
    Token->Type         = INIT;
    Token->Value.Index  = -1;
}

static char* SkipSpaces(char* current_ptr, char* end_ptr)
{
    size_t end_position = 0;
    MYASSERT(current_ptr, ERR_BAD_POINTER_DATA, return 0)
    while (((current_ptr + end_position) < end_ptr) && (*(current_ptr + end_position) == '\0')) 
    {
         end_position++; 
    }
    if ((current_ptr + end_position) == end_ptr) return NULL;
    return current_ptr + end_position;
}

static void SkipComments(char* start_ptr, char* end_ptr)
{
    //Process one-line comment
    bool start_comment = 0;
    size_t i = 0;
    while ((start_ptr + i) < end_ptr)
    {
        if ((*(start_ptr + i) == COMMENT_SYMBOL_UNO) && !start_comment) 
        {
            *(start_ptr + i) = '\0';
            start_comment = 1;
            i++;
            continue;
        }
        if (((*(start_ptr + i) == '\n') || (*(start_ptr + i) == '\0')) && start_comment) 
        {
            *(start_ptr + i) = '\0';   
            start_comment = 0;
        }
        if (start_comment) *(start_ptr + i) = '\0';
        i++;
    }
    //Process multy comment
    start_comment = 0;
    i = 0;
    while ((start_ptr + i) < end_ptr)
    {
        if ((*(start_ptr + i) == COMMENT_SYMBOL_DUO) && !start_comment) 
        {
            *(start_ptr + i) = '\0';   
            start_comment = 1;
        }
        if ((*(start_ptr + i) == COMMENT_SYMBOL_DUO) && start_comment) 
        {
            *(start_ptr + i) = '\0';   
            start_comment = 0;
        }
        if (start_comment) *(start_ptr + i) = '\0';
        i++;
    }
}

static void PutNull(char* start_ptr, char* end_ptr)
{
    size_t i = 0;
    while ((start_ptr + i) <= end_ptr)
    {
        if (isspace(*(start_ptr + i))) *(start_ptr + i) = '\0';
        i++;
    }
}

static bool CompareVar(const char* str1, const char* str2, size_t num)
{
    size_t i = 0;
    for (i = 0; (isalnum(str1[i]) || (str1[i] == '_')) && (isalnum(str2[i]) || (str2[i] == '_')) && (i < num) ; ++i)
    {
        if (str1[i] != str2[i]) 
        {  
            return 0;
        }
    }

    if ((!(isalnum(str1[i]) || (str1[i] == '_')) && (isalnum(str2[i]) || (str2[i] == '_'))) || ((isalnum(str1[i]) || (str1[i] == '_')) && !(isalnum(str2[i]) || (str2[i] == '_'))))
        return 0;

    return 1;
}

static int StrInOperators(const char* str)
{
    size_t token_size = TokenSize(str);
    for (size_t i = 0; i < SIZE_OF_OPERATORS; i++)
    {
        if ((!strncmp(ArrayOperators[i].Name, str, strlen(ArrayOperators[i].Name))) && (ArrayOperators[i].Num == MULT)) return (int) i;
        if ((!strncmp(ArrayOperators[i].Name, str, strlen(ArrayOperators[i].Name))) && (strlen(ArrayOperators[i].Name) == token_size) && (ArrayOperators[i].Num == SOLO)) return (int) i;
    }
    return NOT_IN_OPER;
}

static size_t TokenSize(const char* str)
{
    EnumOperType type = UNKNOWN;
    type = TypeOfChar(*str);
    size_t i = 0;
    while ((TypeOfChar(*(str + i)) == type) && (*(str + i) != '\0'))
    {
        i++;
    }
    return i;
}

static EnumOperType TypeOfChar(char unknown_char)
{
    if (isalnum(unknown_char)) return LETTER;
    else                       return SYMBOL;
}

void PrintToken(Token_t* CurrToken)
{
    if (CurrToken->Type == NUMBER)   printf("[num   %lg]\n", CurrToken->Value.Number);
    if (CurrToken->Type == OPERATOR) printf("[oper  %s]\n",  ArrayOperators[CurrToken->Value.Index].Name);
    if (CurrToken->Type == VARIABLE) printf("[var   %s]\n",  "now");
    if (CurrToken->Type == END)      printf("[end     ]\n");
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "colors.h"
#include "tree.h"
#include "tree_functions.h"
#include "log.h"
#include "myassert.h"
#include "verificator.h"
#include "tokenizer.h"

static const int    NOT_IN_OPER   = -1;
static const size_t MULTIPLIER    = 2;
static const size_t INIT_CAPACITY = 10;

static FILE*     OpenFile           (const char* file_open, const char* option);
static void      CloseFile          (FILE* file_text);
static size_t    FileSize           (FILE *file_text);
static void      InitToken          (Token_t* Token);
static int       MemPoison          (void* memptr, size_t num);
static Token_t*  Expand             (Tokens_t *TokensArr);
static int       StrInOperators     (const char* str);
static int       StrInBrackets      (const char* str);
static char*     SkipSpaces         (char* current_ptr, char* end_ptr);
static void      SkipComments       (char* buffer, char* end_ptr);
static void      PutNull            (char* buffer);
static char*     ProcessVariable    (Tokens_t* TokensArr, char* current_ptr);
static int       InsertVariable     (Tokens_t* TokensArr, char* current_ptr);
static char*     ProcessNumber      (Tokens_t* TokensArr, char* current_ptr, bool* token_continue);
static char*     ProcessOperator    (Tokens_t* TokensArr, char* current_ptr, bool* token_continue);
static char*     ProcessBracket     (Tokens_t* TokensArr, char* current_ptr, bool* token_continue);
static EnumOperType TypeOfChar      (char unknown_char);
static size_t       TokenSize       (const char* str);
static bool         CheckIfVar      (const char* maybe_var);
//========================================================================================================
//========================================================================================================
//Создание текстового массива кода на ЯП

char* CreateDirtyBuffer (const char* file_database)
{
    MYASSERT(file_database, ERR_WHAT_FILE_OF_DATA, return NULL)

    FILE* FileRead = OpenFile (file_database, "rb");
    USER_ERROR(FileRead, ERR_NO_FILE_TO_OPEN, '\0', exit(0))

    size_t size_text = FileSize (FileRead);

    char* text_buffer = NULL;
    text_buffer = (char*) calloc (size_text + 1, sizeof (char));
    MYASSERT(text_buffer, ERR_BAD_CALLOC, return NULL)
    size_t result_size = fread (text_buffer, 1, size_text, FileRead);

    MYASSERT(result_size == size_text, ERR_BAD_FREAD, return NULL);

    if (*text_buffer == '\0') 
    {
        fprintf(stdout, YELLOW "Data base is empty!\n" RESET);
        return text_buffer;
    }

	*(text_buffer + size_text) = '\0';
    CloseFile (FileRead);

    return text_buffer;   
}

static FILE* OpenFile (const char* file_open, const char* option)
{
    FILE *FileOpen = fopen (file_open, option);

    MYASSERT(FileOpen, OPEN_FILE, return NULL);

    return FileOpen;
}

static void CloseFile (FILE* file_text)
{
	MYASSERT(file_text, BAD_POINTER_PASSED_IN_FUNC, assert(0));
    int result = fclose(file_text);
	MYASSERT(!result, CLOSE_FILE, assert(0));
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
//========================================================================================================
//Создание массива токенов

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
    MYASSERT(dirty_buffer, ERR_BAD_POINTER_PASSED_IN_FUNC, return ERR_BAD_POINTER_PASSED_IN_FUNC)
    bool token_continue = 0;
    char* current_ptr = dirty_buffer;
    size_t buffer_size = 0;
    while (*(current_ptr + buffer_size) != '\0')
    {
        buffer_size++;
    }
    SkipComments(current_ptr, dirty_buffer + buffer_size);
    PutNull(current_ptr);
    while (current_ptr < (dirty_buffer + buffer_size))
    {
        current_ptr = SkipSpaces(current_ptr, dirty_buffer + buffer_size); //пропустили isspace
        //обработка числа
        current_ptr = ProcessNumber(TokensArr, current_ptr, &token_continue);
        //обработка спец оператора
        current_ptr = ProcessOperator(TokensArr, current_ptr, &token_continue);
        //обработка скобок
        current_ptr = ProcessBracket(TokensArr, current_ptr, &token_continue);
        //потом уже переменная
        if (!token_continue) 
        {
            USER_ERROR(CheckIfVar(current_ptr), ERR_UNKNOWN_OBJ, *current_ptr, exit(0))
            current_ptr = ProcessVariable(TokensArr, current_ptr);
        }
        token_continue = 0;
    }
    (TokensArr->Data + TokensArr->Size)->Type        = END;
    (TokensArr->Data + TokensArr->Size)->Value.Index = 0;
    (TokensArr->Size)++;
    return ERR_OK;
}

static bool CheckIfVar(const char* maybe_var)
{
    if (isalpha(*maybe_var)) return true;
    return false;    
}

static char* ProcessVariable(Tokens_t* TokensArr, char* current_ptr)
{
    char* new_ptr = current_ptr;
    int index = InsertVariable (TokensArr, current_ptr);
    if (TokensArr->Size == (int) TokensArr->Capacity) //expansion
    {
        TokensArr->Data = Expand(TokensArr);
    }
    (TokensArr->Data + TokensArr->Size)->Type         = VARIABLE;
    (TokensArr->Data + TokensArr->Size)->Value.Index = index;
    (TokensArr->Size)++;
    USER_ERROR(index != -1, ERR_OVERFLOW_VARIABLES, '_', exit(0))
    size_t size = strlen(TokensArr->Variables[index]);
    new_ptr = new_ptr + size;
    return new_ptr;
}

static int InsertVariable (Tokens_t* TokensArr, char* current_ptr)
{
    for (size_t i = 0; i < SIZE_OF_VARIABLES; i++)
    {
        if (!strncmp(TokensArr->Variables[i], current_ptr, SIZE_OF_VAR))
        {
            return (int) i;
        }
        if (!(*(TokensArr->Variables[i]))) //ищем свободное место в массиве переменных
        {
            //копирка
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
    double number = strtod(current_ptr, &new_ptr);
    if (current_ptr != new_ptr) //creating token with number
    {
        if (TokensArr->Size == (int) TokensArr->Capacity) //expansion
        {
            TokensArr->Data = Expand(TokensArr);
        }
        (TokensArr->Data + TokensArr->Size)->Type          = NUMBER;
        (TokensArr->Data + TokensArr->Size)->Value.Number = number;
        (TokensArr->Size)++;
        *token_continue = 1;
    }
    return new_ptr;
}

static char* ProcessOperator(Tokens_t* TokensArr, char* current_ptr, bool* token_continue)
{
    char* new_ptr = current_ptr;
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

        //сдвиг указателя
        size_t size = strlen(ArrayOperators[index].Name);
        new_ptr = new_ptr + size;
    }
    return new_ptr;
}

static char* ProcessBracket(Tokens_t* TokensArr, char* current_ptr, bool* token_continue)
{
    char* new_ptr = current_ptr;
    int index = StrInBrackets(current_ptr);
    if (index != NOT_IN_OPER) //creating token with bracket
    {
        if (TokensArr->Size == (int) TokensArr->Capacity) //expansion
        {
            TokensArr->Data = Expand(TokensArr);
        }
        (TokensArr->Data + TokensArr->Size)->Type        = BRACKET;
        (TokensArr->Data + TokensArr->Size)->Value.Index = index;
        (TokensArr->Size)++;
        *token_continue = 1;

        //сдвиг указателя
        size_t size = strlen(ArrayBrackets[index]);
        new_ptr = new_ptr + size;
    }
    return new_ptr;
}

static Token_t* Expand (Tokens_t *TokensArr)
{
    MYASSERT(TokensArr, BAD_POINTER_PASSED_IN_FUNC, return NULL)
    void* new_place = nullptr;
    TokensArr->Capacity = (TokensArr->Capacity) * MULTIPLIER;                                                                                      
    new_place = realloc((TokensArr->Data), sizeof(Token_t)*(TokensArr->Capacity));

    MYASSERT(new_place, BAD_REALLOC, return NULL)
                                                 
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
    while (((current_ptr + end_position) < end_ptr) && *(current_ptr + end_position) == '\0') 
    {
        end_position++; 
    }
    return current_ptr + end_position;
}

static void SkipComments(char* buffer, char* end_ptr)
{
    bool start_comment = 0;
    size_t i = 0;
    while ((buffer + i) < end_ptr)
    {
        if ((*(buffer + i) == COMMENT_SYMBOL) && !start_comment) 
        {
            *(buffer + i) = '\n';   
            start_comment = 1;
        }
        if ((*(buffer + i) == COMMENT_SYMBOL) && start_comment) 
        {
            *(buffer + i) = '\n';   
            start_comment = 0;
        }
        if (start_comment) *(buffer + i) = '\n';
        i++;
    }
}

static void PutNull(char* buffer)
{
    size_t i = 0;
    while (*(buffer + i) != '\0')
    {
        if (isspace(*(buffer + i))) *(buffer + i) = '\0';
        i++;
    }
}

static int StrInOperators(const char* str)
{
    size_t token_size = TokenSize(str);
    for (size_t i = 0; i < SIZE_OF_OPERATORS; i++)
    {
        if ((!strncmp(ArrayOperators[i].Name, str, strlen(ArrayOperators[i].Name))) && (strlen(ArrayOperators[i].Name) == token_size)) return (int) i;
    }
    return NOT_IN_OPER;
}

static int StrInBrackets(const char* str)
{
    for (size_t i = 0; i < SIZE_OF_BRACKETS; i++)
    {
        if (!strncmp(ArrayBrackets[i], str, 1)) return (int) i;
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
//========================================================================================================
//========================================================================================================

#ifndef TOKENIZER_H
#define TOKENIZER_H

//==============================================================================================================
//TOKEN

typedef struct TokenValue {
    Elem_t Number;
    int    Index;
} TokenValue_t;

typedef struct Token {
    EnumOfType Type;
    TokenValue Value;
} Token_t;

typedef struct Tokens {
    Token_t* Data;
    size_t   Capacity;
    int      Size;
    char     Variables[SIZE_OF_VARIABLES][SIZE_OF_VAR];
} Tokens_t;

//==============================================================================================================
//FUNCTIONS

char*           CreateDirtyBuffer (const char* file_database);
void            TokensCtor (Tokens_t *TokensArr);
void            TokensDtor (Tokens_t *TokensArr);
EnumOfErrors    CreateTokens(Tokens_t* TokensArr, char* dirty_buffer);
void            PrintToken(Token_t* CurrToken);

#endif
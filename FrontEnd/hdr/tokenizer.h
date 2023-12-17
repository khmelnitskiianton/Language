#ifndef TOKENIZER_H
#define TOKENIZER_H

char* CreateDirtyBuffer (const char* file_database);
void  TokensCtor (Tokens_t *TokensArr);
void  TokensDtor (Tokens_t *TokensArr);
EnumOfErrors CreateTokens(Tokens_t* TokensArr, char* dirty_buffer);

#endif
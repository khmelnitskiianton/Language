#ifndef CREATOR_H
#define CREATOR_H

const int NOT_IN_OPER = -1;

EnumOfErrors UploadTree         (BinaryTree_t* myTree, const char* file_database);
char*        CreateDirtyBuffer  (const char* file_database);
 void        CleanCharBuffer    (char* buffer, const size_t buffer_size);
FILE*        OpenFile           (const char* file_open, const char* option);
void         CloseFile          (FILE* file_text);
size_t       FileSize           (FILE *file_text);

#endif
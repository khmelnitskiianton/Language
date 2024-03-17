#ifndef CREATOR_H
#define CREATOR_H

const int NOT_IN_OPER = -1;

EnumOfErrors UploadTree (BinaryTree_t* myTree, const char* file_database);
char* CreateDirtyBuffer (const char* file_database);

#endif
#ifndef TRANSLATOR_H
#define TRANSLATOR_H

EnumOfErrors TranslateTree(BinaryTree_t* myTree, const char* in_file_path, StackInt_t* StackWhileCond);

#define WHILE_TYPE 0
#define IF_TYPE 1

#endif
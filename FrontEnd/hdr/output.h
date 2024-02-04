#ifndef OUTPUT_H
#define OUTPUT_H

#define FILE_TREE "tree.txt"
#define FILE_VARS "vars.txt"

EnumOfErrors PrintTree (BinaryTree_t* myTree, const char* out_file_path);

const size_t SIZE_OF_OUT_PATH    = 500;
const size_t SIZE_OF_OUT_COMMAND = SIZE_OF_OUT_PATH + 40;

#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "colors.h"
#include "tree.h"
#include "MyLangConfig.h"
#include "creator.h"
#include "log.h"
#include "myassert.h"
#include "verificator.h"

static EnumOfErrors RecScanTree     (size_t* position, const char* text_buffer, Node_t** ResNode, Node_t* CurrentNode, BinaryTree_t* myTree);
static size_t       SkipSpaces      (size_t current_position, const char* text_buffer);
static EnumOfErrors ScanVars        (char* vars_buffer, BinaryTree_t* myTree);
static EnumOfErrors ProcessObject   (char* object_buffer, Node_t* NewNode);
static void         CleanCharBuffer (char* buffer, const size_t buffer_size);

bool global_bool_verify = 0;    //0 - everything ok, if error - bool = 1

EnumOfErrors UploadTree (BinaryTree_t* myTree, const char* file_database)
{
    MYASSERT(myTree, ERR_BAD_POINTER_TREE, return ERR_BAD_POINTER_TREE)
    MYASSERT(file_database, ERR_WHAT_FILE_OF_DATA, return ERR_WHAT_FILE_OF_DATA)

    FILE* FileRead = OpenFile (file_database, "rb");
    USER_ERROR(FileRead, ERR_NO_FILE_TO_OPEN, "",exit(0))

    size_t size_text = FileSize (FileRead);

    char* text_buffer = NULL;
    text_buffer = (char*) calloc (size_text + 2, sizeof (char));
    MYASSERT(text_buffer, ERR_BAD_CALLOC, return ERR_BAD_CALLOC)
    size_t result_size = fread (text_buffer, 1, size_text, FileRead);

    MYASSERT(result_size == size_text, ERR_BAD_FREAD, return ERR_BAD_FREAD);

    USER_ERROR(*text_buffer != '\0', ERR_EMPTY_FILE, "",return ERR_EMPTY_FILE)
    
    *(text_buffer + size_text) = '\n';      //for scanning vars!
	*(text_buffer + size_text + 1) = '\0';
    CloseFile (FileRead);

    //printf("\n%s\n", text_buffer);
    EnumOfErrors return_val = ERR_OK;
    //1) Scan Tree
    size_t position = 0;    //position in buffer
    char* vars_buffer = NULL;
    return_val = RecScanTree(&position, text_buffer, NULL, myTree->Root, myTree);
    //if error - jmp to error close
    if (return_val != ERR_OK)
    {
        goto end_label;
    }
    //2) Scan vars
    vars_buffer = strchr(text_buffer, '\n');
    vars_buffer++;
    return_val = ScanVars(vars_buffer, myTree);
end_label:
    free(text_buffer);
    return return_val;   
}

FILE* OpenFile (const char* file_open, const char* option)
{
    FILE *FileOpen = fopen (file_open, option);

    MYASSERT(FileOpen, OPEN_FILE, return NULL);

    return FileOpen;
}

void CloseFile (FILE* file_text)
{
	MYASSERT(file_text, BAD_POINTER_PASSED_IN_FUNC, assert(0));
    int result = fclose(file_text);
	MYASSERT(!result, CLOSE_FILE, assert(0));
}

size_t FileSize (FILE *file_text)
{
    MYASSERT(file_text, ERR_BAD_POINTER_FILE, return 0)
	struct stat st;
    int fd = fileno (file_text); 
    fstat (fd, &st);
    size_t size_text = (size_t) st.st_size;
	return size_text;
}

static EnumOfErrors RecScanTree(size_t* position, const char* text_buffer, Node_t** ResNode, Node_t* CurrentNode, BinaryTree_t* myTree)
{
    if (global_bool_verify == 1)
    {
        return ERR_UNKNOWN_OBJ;
    }
    //printf("Найдено: %c %d\n", *(text_buffer + *position), *position);
    *position = SkipSpaces(*position, text_buffer); //пропускаем пробелы
    if (!strncmp((text_buffer + *position), "_", 1))
    {
        //subtree - empty
        *position = *position + 1;
        *position = SkipSpaces(*position, text_buffer);
        return ERR_OK;
    }
    //find open bracket
    if (*(text_buffer + *position) == '(')
    {
        //create new node
        char object_buffer[SIZE_OF_BUFFER] = {};
        Node_t* NewNode = CreateNode(myTree);

        (*position)++;
        *position = SkipSpaces (*position, text_buffer); //пропускаем пробелы

        EnumOfErrors result = ERR_OK; 

        //1. Scan Value
        size_t shift = 0;
        //put in buffer object
        while ((isdigit(*(text_buffer + *position)))||(isspace(*(text_buffer + *position)))||(*(text_buffer + *position) == '.')) 
        {
            //printf("Найден2: %c %d\n", *(text_buffer + *position), *position);
            if (shift == SIZE_OF_BUFFER){
                USER_ERROR(0, ERR_OVERFLOW_BUFF, "",global_bool_verify = 1)
                RecFree(NewNode);
                return ERR_OVERFLOW_BUFF;
            }
            *(object_buffer + shift) = *(text_buffer + *position);
            shift++;
            (*position)++;
        }
        //printf("Найден3: %c %d\n", *(text_buffer + *position), *position);
        result = ProcessObject(object_buffer, NewNode); //обработка значения прочитанного
        if(result != ERR_OK)
        {
            RecFree(NewNode);
            return result;
        }
        CleanCharBuffer(object_buffer, SIZE_OF_BUFFER);
        *position = SkipSpaces (*position, text_buffer);

        //Linking NewNode
        NewNode->Parent = CurrentNode;//подвязка от предыдущего
        if (CurrentNode)//проверка на корень
        {
            *ResNode = NewNode; //подвязка прошлого к новому
        }
        else
        {
            myTree->Root = NewNode;
        }

        //3. Scan left subtree
        result = RecScanTree(position, text_buffer, &NewNode->Left, NewNode, myTree);
        if (global_bool_verify == 1)
        {
            RecFree(NewNode);
            return result;
        }
        *position = SkipSpaces (*position, text_buffer);
        //4. Scan right subtree
        result = RecScanTree(position, text_buffer, &NewNode->Right, NewNode, myTree);
        if (global_bool_verify == 1)
        {
            RecFree(NewNode);
            return result;
        }
        *position = SkipSpaces (*position, text_buffer);
        
        //printf("Найден4: %c %d\n", *(text_buffer + *position), *position);
        //4. Close node
        if (*(text_buffer + *position) == ')')
        {
            (*position)++;
            return ERR_OK;
        }
        else
        {
            USER_ERROR(0, ERR_NO_CLOSE_BRACKET, "",global_bool_verify = 1)
            RecFree(NewNode);
            return ERR_NO_CLOSE_BRACKET;
        }
    }
    else
    {
        USER_ERROR(0, ERR_NO_OPEN_BRACKET, "",global_bool_verify = 1)
        return ERR_NO_OPEN_BRACKET;
    }
}

static size_t SkipSpaces(size_t current_position, const char* text_buffer)
{
    size_t end_position = current_position;
    MYASSERT(text_buffer, ERR_BAD_POINTER_DATA, return 0)
    while (isspace(*(text_buffer + end_position)) && (*(text_buffer + end_position) != '\0')) { end_position++; }
    return end_position;
}

static EnumOfErrors ProcessObject (char* object_buffer, Node_t* NewNode)
{
    //in buffer two numbers "1 2"
    unsigned int type_node = 0;
    char* ptr_arg = object_buffer;  //ptr_arg - pointer on argument in buffer for sscanf
    while((*ptr_arg != ' ')&&(*ptr_arg != '\0'))
    {
        ptr_arg++;
    }
    //scan type of arg
    if (sscanf(object_buffer, " %u ", &type_node) != 1)  //если цифра 100% число
    {
        USER_ERROR(0, ERR_BAD_VALUE_OF_DATA, "",global_bool_verify = 1)
        return ERR_BAD_VALUE_OF_DATA;
    }
    //scan number
    double number_arg = NAN;
    if (type_node == NUMBER)
    {
        if (sscanf(ptr_arg, " %lf ", &number_arg) != 1)  //если цифра 100% число
        {
            USER_ERROR(0, ERR_BAD_VALUE_OF_DATA, "", global_bool_verify = 1)
            return ERR_BAD_VALUE_OF_DATA;
        }
        NewNode->Value.Number = number_arg;
        NewNode->Type         = NUMBER;
        return ERR_OK;
    }
    int index_arg = -1;
    if (sscanf(ptr_arg, " %d ", &index_arg) != 1)
    {
        USER_ERROR(0, ERR_BAD_VALUE_OF_DATA, "", global_bool_verify = 1)
        return ERR_BAD_VALUE_OF_DATA;
    }
    if (type_node == VARIABLE)
    {
        NewNode->Value.Index  = index_arg;
        NewNode->Type         = VARIABLE;
        return ERR_OK;
    }
    if (type_node == OPERATOR)
    {
        int index_oper = -1;
        for (int i = 0; i < (int) SIZE_OF_OPERATORS; i++)
        {
            if ((int)ArrayOperators[i].Id == index_arg) {index_oper = i; break;}
        }
        NewNode->Value.Index  = index_oper;
        NewNode->Type         = OPERATOR;
        return ERR_OK;
    }
    USER_ERROR(0, ERR_BAD_VALUE_OF_DATA, "", global_bool_verify = 1)
    return ERR_BAD_VALUE_OF_DATA;
}

static EnumOfErrors ScanVars(char* vars_buffer, BinaryTree_t* myTree)
{
    char* current_position = vars_buffer;
    char* end_buffer = strchr(vars_buffer, '\0');
    int  index_var = 0;
    int  counter   = 0; 
    char name_buffer[SIZE_OF_VAR] = {};
    while (current_position < end_buffer)
    {
        if (sscanf(current_position, "%d %s\n", &index_var, name_buffer) != 2)
        {
            USER_ERROR(0, ERR_BAD_VARS, "", return ERR_BAD_VARS)
        }
        USER_ERROR(counter == index_var, ERR_BAD_VARS_INDEX, "", return ERR_BAD_VARS_INDEX)
        if (myTree->Variables[index_var].Name[0] != 0)
        {
            USER_ERROR(0, ERR_BAD_VARS_INDEX, "", return ERR_BAD_VARS_INDEX)
        }
        strncpy(myTree->Variables[index_var].Name, name_buffer, SIZE_OF_VAR);
        current_position = strchr(current_position, '\n');
        current_position++;
        counter++;
    }
    return ERR_OK;
}

static void CleanCharBuffer(char* buffer, const size_t buffer_size)
{
    for (size_t i = 0; i < buffer_size; i++)
    {
        *(buffer + i) = 0;
    }
}

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
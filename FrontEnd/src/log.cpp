#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>

#include "tree.h"
#include "tokenizer.h"
#include "parser.h"
#include "log.h"
#include "myassert.h"

static size_t NumImage  = 0;
static char   buffer_path[SIZE_OF_PATH] = {};
static FILE*  FileLog   = NULL;
static FILE*  FileGraph = NULL;
static size_t NullNodes = 0;

static FILE* GenerateImage (void);
static void  GenerateGraph (BinaryTree_t* myTree);
static void  WriteNode     (Node_t* CurrentNode, BinaryTree_t* myTree);
static void  WriteHead     (BinaryTree_t* myTree);
static void  WriteTree     (BinaryTree_t* myTree);
static FILE* OpenFile      (const char* file_open, const char* option);
static void  CloseFile     (FILE* file_text);
static void  CleanCharBuffer(char* buffer, const size_t buffer_size);

EnumOfErrors _PrintLogStart (const char* log_file_path, const char* path_of_code)
{
    MYASSERT(log_file_path, ERR_BAD_OPEN_FILE, return ERR_BAD_POINTER_PASSED_IN_FUNC)
    snprintf(buffer_path, SIZE_OF_PATH, "%s/" FOLDER_LOG "/", log_file_path);
    struct stat sb = {};
    char buffer_create[SIZE_OF_COMMAND] = {};
    if (stat(buffer_path, &sb) || !S_ISDIR(sb.st_mode)) 
    { 
        snprintf(buffer_create, SIZE_OF_COMMAND, "mkdir %s", buffer_path);
        system(buffer_create);
        CleanCharBuffer(buffer_create, SIZE_OF_COMMAND);
        snprintf(buffer_create, SIZE_OF_COMMAND, "touch %s" FILE_GRAPH, buffer_path);
        system(buffer_create);
        CleanCharBuffer(buffer_create, SIZE_OF_COMMAND);
        snprintf(buffer_create, SIZE_OF_COMMAND, "touch %s" FILE_LOG, buffer_path);
        system(buffer_create);
        CleanCharBuffer(buffer_create, SIZE_OF_COMMAND);
    }

    char buffer_file[SIZE_OF_COMMAND] = {};
    snprintf(buffer_file, SIZE_OF_COMMAND, "%s" FILE_LOG, buffer_path);
    FileLog = OpenFile (buffer_file, "w");

    char* code_buffer = CreateDirtyBuffer(path_of_code);
    if (!code_buffer) return ERR_NO_FILE_TO_OPEN;

    fprintf(FileLog,    "<!DOCTYPE html>\n"
                        "<html lang=\"eng\">\n"
                        "<head>\n"
                        "<meta charset = \"utf-8\"/>\n"
                        "<title>LOGS</title>\n"
                        "<style>\n"
                        "body {\n"
                        "\tbackground-color: " FILL_BACK_COLOR ";\n"
                        "}\n"
                        "</style>\n"
                        "</head>\n"
                        "<body>\n"
                        "<pre>\n"
                        "<header>\n"
                        "<h1> Logs for Binary Tree </h1>\n"
                        "</header>\n"
                        "<main>\n"
                        "<pre>\n"
                        "<p>\n"
                        "<b>\n"
                        "<big>\n"
                        "<ins>Code:</ins>\n\n"
                        ">>>\n%s\n<<<\n"
                        "</big>\n"
                        "</b>\n"
                        "<hr size = \"4\" color = \"#000000\">\n" 
                        "</p>\n\n",
                        code_buffer); 
    free(code_buffer);
    return ERR_OK;
}

void _PrintLogFinish (void)
{
    fprintf(FileLog,    "</pre>\n"
                        "</main>\n"
                        "</body>\n"
                        "</html>\n"); 
    CloseFile (FileLog);
}

void _PrintLogTree (BinaryTree_t* myTree, const char* file,  const char* function, const size_t line)
{
    GenerateGraph (myTree);
    FILE* pPipe = GenerateImage ();

    fprintf(FileLog,    "\n<p>\n"
                        "<b><big> ### In file: %s,\tIn function: %s,\tIn line: %lu ### </big></b>\n", file, function, line);

    size_t counter_strings = 0;
    while (counter_strings < 6)
    {
        if (fgetc(pPipe) == '\n')
        {
            counter_strings++;
        }
    }
    int symbol = fgetc(pPipe);

    while (symbol != EOF)
    {
        fputc(symbol, FileLog);
        symbol = fgetc(pPipe);
    }

    //"<img src=\"images/%lu." TYPE_OF_IMAGE "\" alt=\"Printing binary tree №%lu\" >\n"
                        
    fprintf(FileLog,    "</p>\n"
                        "<hr size = \"4\" color = \"#000000\">\n\n");
    (NumImage)++;
}

void  _PrintLogText (const char* log_string, const char* file,  const char* function, const size_t line)
{
    fprintf(FileLog,    "\n<p>\n"
                        "<b>## MESSAGE: in %s\t\t%s:%lu\t\t[%s]</b>\n", function, file, line, log_string);
    fprintf(FileLog,    "</p>\n");
}


static void GenerateGraph (BinaryTree_t* myTree)
{
    char buffer_file[SIZE_OF_COMMAND] = {};
    snprintf(buffer_file, SIZE_OF_COMMAND, "%s" FILE_GRAPH, buffer_path);
    FileGraph = OpenFile(buffer_file, "w");

    fprintf (FileGraph,     "digraph G{\n"
                            "rankdir = TB;\n"
                            "bgcolor = \"" FILL_BACK_COLOR "\";\n"
                            "node[color = \"" COLOR_FRAME "\", fontsize = 10];\n"
                            "edge[color = \"" COLOR_FRAME "\", fontsize = 15];\n"

                            "subgraph cluster_list {\n"
                            "\tnode[style = filled, fontsize = 10];\n"
                            "\tedge[color = \"" COLOR_FRAME "\", fontsize = 15];\n"
                            "\tstyle = \"dashed\";\n"
                            "\tcolor = \"" COLOR_STIPPLE "\";\n");

    WriteTree(myTree);

    fprintf (FileGraph,     "}\n");

    WriteHead(myTree);

    fprintf (FileGraph,     "}\n");

    CloseFile (FileGraph);
} 

static FILE* GenerateImage (void)
{
    char buffer_cmd[SIZE_OF_COMMAND] = {};
    snprintf(buffer_cmd, SIZE_OF_COMMAND, "dot %s" FILE_GRAPH " -T " TYPE_OF_IMAGE, buffer_path);
    FILE* pPipe = popen (buffer_cmd, "r");
    MYASSERT(pPipe, ERR_BAD_OPEN_FILE, )
    return pPipe;
}

static FILE* OpenFile (const char* file_open, const char* option)
{
    FILE *FileOpen = fopen (file_open, option);

    MYASSERT(FileOpen, ERR_OPEN_FILE, return 0);

    return FileOpen;
}

static void CloseFile (FILE* file_text)
{
	MYASSERT(file_text, ERR_BAD_POINTER_PASSED_IN_FUNC, assert(0));
    int result = fclose(file_text);
	MYASSERT(!result, ERR_CLOSE_FILE, assert(0));
}

static void WriteTree (BinaryTree_t* myTree)
{
    if (!(myTree->Root)) 
    {
        return;
    }
    else
    {
        WriteNode (myTree->Root, myTree);
        NullNodes = 0;
    }
}

static void WriteNode (Node_t* CurrentNode, BinaryTree_t* myTree)
{
    if (CurrentNode->Type == INIT) 
    {                               
        fprintf (FileGraph, "\tnode%p [shape = Mrecord, style = filled, fillcolor = \"" COLOR_INIT "\", color = \"" COLOR_FRAME "\", label = \"{ PARENT: %p | PTR: %p | TYPE: %d | DATA: " SPECIFIER_INIT  " | { <f0> LEFT: %p | <f1> RIGHT: %p }}\"];\n", 
                CurrentNode, CurrentNode->Parent, CurrentNode, INIT, CurrentNode->Value.Number, CurrentNode->Left, CurrentNode->Right);                                                                                                                                              
    }
    if (CurrentNode->Type == NUMBER) 
    {                               
        fprintf (FileGraph, "\tnode%p [shape = Mrecord, style = filled, fillcolor = \"" COLOR_NUMBER "\", color = \"" COLOR_FRAME "\", label = \"{ PARENT: %p | PTR: %p | TYPE: %d | DATA: " SPECIFIER_NUMBER  " | { <f0> LEFT: %p | <f1> RIGHT: %p }}\"];\n", 
                CurrentNode, CurrentNode->Parent, CurrentNode, NUMBER, CurrentNode->Value.Number, CurrentNode->Left, CurrentNode->Right);                                                                                                                                              
    }
    if (CurrentNode->Type == OPERATOR) 
    {
        fprintf (FileGraph, "\tnode%p [shape = Mrecord, style = filled, fillcolor = \"" COLOR_OPERATOR "\", color = \"" COLOR_FRAME "\", label = \"{ PARENT: %p | PTR: %p | TYPE: %d | DATA: " SPECIFIER_OPERATOR_STR  " | { <f0> LEFT: %p | <f1> RIGHT: %p }}\"];\n", 
                CurrentNode, CurrentNode->Parent, CurrentNode, OPERATOR, ArrayOperators[CurrentNode->Value.Index].Name, CurrentNode->Left, CurrentNode->Right);                                                                                                                                              
    }
    if (CurrentNode->Type == VARIABLE) 
    {                               
        fprintf (FileGraph, "\tnode%p [shape = Mrecord, style = filled, fillcolor = \"" COLOR_VARIABLE "\", color = \"" COLOR_EDGE_GRAPH "\", label = \"{ PARENT: %p | PTR: %p | TYPE: %d | DATA: " SPECIFIER_VARIABLE_STR " | { <f0> LEFT: %p | <f1> RIGHT: %p }}\"];\n", 
                CurrentNode, CurrentNode->Parent, CurrentNode, VARIABLE, myTree->Variables[CurrentNode->Value.Index], CurrentNode->Left, CurrentNode->Right);                                                                
    }
    if (CurrentNode->Left)
    {
        WriteNode (CurrentNode->Left, myTree);
        fprintf (FileGraph, "\tnode%p: <f0> -> node%p [color = \"" COLOR_EDGE_GRAPH "\"]\n"
                , CurrentNode, CurrentNode->Left);
    }
    else
    {
        NullNodes++;
    }
    if (CurrentNode->Right)
    {
        WriteNode (CurrentNode->Right, myTree);
        fprintf (FileGraph, "\tnode%p: <f1> -> node%p [color = \"" COLOR_EDGE_GRAPH "\"]\n", CurrentNode, CurrentNode->Right);
    }
    else
    {
        NullNodes++;
    }
}

static void WriteHead(BinaryTree_t* myTree)
{
    fprintf (FileGraph, "All[shape = Mrecord, label = \" HEADER | <f0> Root: %p \", style = \"filled\", fillcolor = \"" FILL_BACK_GRAPH "\"];\n", myTree->Root);
    if (myTree->Root)
    {
        fprintf (FileGraph, "All:<f0> -> node%p [color = \"" COLOR_EDGE_HEAD "\"];\n", myTree->Root);                                           
    }
}

static void CleanCharBuffer(char* buffer, const size_t buffer_size)
{
    for (size_t i = 0; i < buffer_size; i++)
    {
        *(buffer + i) = 0;
    }
}
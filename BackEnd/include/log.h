#ifndef LOG_B_E_H
#define LOG_B_E_H

#define DEBUG_LOG 1

#ifdef DEBUG_LOG
    #define PrintLogStart(path, path1) \
            _PrintLogStart(path, path1)
    #define PrintLogFinish() \
            _PrintLogFinish()
    #define PrintLogTree(myTree) \
            _PrintLogTree(myTree, __FILE__, __PRETTY_FUNCTION__, __LINE__)
    #define PrintLogText(string) \
			_PrintLogText(string, __FILE__, __PRETTY_FUNCTION__, __LINE__)
#else
    #define PrintLogTree(myTree)
    #define PrintLogStart(path)
    #define PrintLogFinish()
	#define PrintLogText(string)
#endif

void            _PrintLogTree (BinaryTree_t* myTree, 
                    const char* file,  const char* function, const size_t line);
EnumOfErrors    _PrintLogStart (const char* log_file_path, const char* path_of_code);
void  	        _PrintLogFinish (void);
void  	        _PrintLogText (const char* log_string, const char* file,  const char* function, const size_t line);

#define FOLDER_LOG "log_backend"
#define FILE_LOG "log.html"
#define FILE_GRAPH "graph.dot"
#define TYPE_OF_IMAGE "svg"

const size_t SIZE_OF_PATH   	= 500;
const size_t SIZE_OF_POINTER 	= 13;
const size_t SIZE_OF_COMMAND 	= SIZE_OF_PATH + 40;

#define FILL_BACK_COLOR "#FFFACD"
#define FILL_BACK_GRAPH "#FFA07A"
#define FILL_BACK_GRAPH_NULL "#BC8F8F"
#define COLOR_FRAME "#000000"
#define COLOR_STIPPLE "#483D8B"
#define COLOR_EDGE_GRAPH "#000000"
#define COLOR_EDGE_HEAD "#000000"

#define COLOR_INIT "#000000"
#define COLOR_NUMBER "#9ACD32"
#define COLOR_OPERATOR "#40E0D0"
#define COLOR_VARIABLE "#ffdb58" 
 
#endif
#ifndef DUMP_H
#define DUMP_H
//SIMPLE DUMP FOR DEBUGGING

#include "colors.h"

#define DEBUG_DUMP 1

#ifdef DEBUG_DUMP
    #define DUMP_PRINT(...)                                                                                     \
    do                                                                                                          \
    {                                                                                                           \
        fprintf(stderr, "In file: %s\tIn line: %d\tIn function: %s, ", __FILE__, __LINE__, __PRETTY_FUNCTION__); \
        fprintf(stderr, __VA_ARGS__);                                                                           \
        fprintf(stderr, RESET);                                                                                 \
    } while (0);

    #define DUMP_VALUE(...)     \
    do                          \
    {                           \
        fprintf(stderr, CYAN);  \
        DUMP_PRINT(__VA_ARGS__);\
    } while (0);

    #define DUMP_ERROR(...)     \
    do                          \
    {                           \
        fprintf(stderr, RED);   \
        DUMP_PRINT(__VA_ARGS__);\
    } while (0);

    #define DUMP_SUCCESS(...)   \
    do                          \
    {                           \
        fprintf(stderr, GREEN); \
        DUMP_PRINT(__VA_ARGS__);\
    } while (0);
#else
    #define DUMP_PRINT(...)         do {} while(0);
    #define DUMP_VALUE(...)         do {} while(0);
    #define DUMP_ERROR(...)         do {} while(0);
    #define DUMP_SUCCESS(...)       do {} while(0);
    #define IF_DUMP_DEBUG(...)      do {} while(0);
#endif

#endif
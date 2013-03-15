#ifndef DEBUG_H
#define DEBUG_H

#include <cstdio>

#define RED     "\033[0;31m"     /* 0 -> normal ;  31 -> red */
#define CYAN    "\033[1;36m"     /* 1 -> bold ;  36 -> cyan */
#define GREEN   "\033[4;32m"     /* 4 -> underline ;  32 -> green */
#define BLUE    "\033[9;34m"     /* 9 -> strike ;  34 -> blue */

#define BLACK   "\033[0;30m"
#define BROWN   "\033[0;33m"
#define MAGENTA "\033[0;35m"
#define GRAY    "\033[0;37m"

#define NONE    "\033[0m"        /* To flush the previous property. */

#define info(...)       {printf(__VA_ARGS__); printf("\n");}
#define error(...)      {printf("%s", RED); printf(__VA_ARGS__); printf("\n%s", NONE);}
#define debug(...)   	{printf("%s", BLUE); printf(__VA_ARGS__); printf("\n%s", NONE);}

#endif

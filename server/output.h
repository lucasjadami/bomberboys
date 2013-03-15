#ifndef DEBUG_H
#define DEBUG_H

#include <signal.h>
#include <cstdio>

#define RED     "\033[0;31m"
#define CYAN    "\033[0;36m"
#define GREEN   "\033[0;32m"
#define BLUE    "\033[0;34m"

#define BLACK   "\033[0;30m"
#define BROWN   "\033[0;33m"
#define MAGENTA "\033[0;35m"
#define GRAY    "\033[0;37m"

#define NONE    "\033[0m"        /* To flush the previous property. */

inline int exit()
{
    raise(SIGINT);
    return 1;
}

#define info(...)       printf(__VA_ARGS__) && printf("\n")
#define error(...)      printf("%s", RED) && printf(__VA_ARGS__) && printf("\n%s", NONE) && exit()
#define debug(...)   	printf("%s", BLUE) && printf(__VA_ARGS__) && printf("\n%s", NONE)

#endif

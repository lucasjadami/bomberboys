#include "output.h"
#include <cstdio>
#include <cstdlib>

void error(const char* message)
{
    	perror(message);
    	exit(1);
}

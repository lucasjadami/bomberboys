#ifndef SERVER_H
#define SERVER_H

#include "settings.h"

extern void launchTestbed(void (*)(Settings*), void (*)(Settings*), int, char**, float, float);

#endif

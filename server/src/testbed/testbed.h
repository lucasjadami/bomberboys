#ifndef TESTBED_H
#define TESTBED_H

#include "settings.h"

extern void launchTestbed(void (*)(Settings*), void (*)(Settings*), int, char**, float, float);

#endif

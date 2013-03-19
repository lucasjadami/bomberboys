#ifndef GAME_TEST_H
#define GAME_TEST_H

#include "test.h"

class GameTest : public Test
{
public:
    static Test*    Create();
    void            Step(Settings*);

};

#endif

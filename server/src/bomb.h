#ifndef BOMB_H
#define BOMB_H

#include <Box2D/Box2D.h>

#define BOMB_TIME           5000
#define BOMB_SIZE           5
#define BOMB_MAX_IMPULSE    200.0f

class Bomb
{
public:

            Bomb(int);
    b2Body* getBody();
    void    setBody(b2Body*);
    bool    explode();
    int     getId();

private:

    b2Body*     body;
    long long   plantedTime;
    int         id;

};

#endif

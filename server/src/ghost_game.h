#ifndef GHOST_GAME_H
#define GHOST_GAME_H

#include "game.h"

class GhostGame : public Game
{
public:

    void    connectionHandler(int, Socket*);
    void    update(float, float, float);

};

#endif

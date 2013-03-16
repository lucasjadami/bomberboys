#ifndef GAME_H
#define GAME_H

#include "player.h"
#include "socket.h"
#include <vector>

class Game
{
public:

        Game();
       ~Game();
   void connectionHandler(int, Socket*);
   void updateGamePackets();

private:

    std::vector<Player*> players;

    void    parseLoginPacket(Packet*, Player*);
    Packet* createAddPlayerPacket(Player*);
    Packet* createRemovePlayerPacket(int);

};

#endif

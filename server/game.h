#ifndef GAME_H
#define GAME_H

#include "player.h"
#include "socket.h"
#include <vector>

#define MAP_WIDTH   600
#define MAP_HEIGHT  420
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
    void    putBytes(char*, int, int);

};

#endif

#ifndef SERVER_H
#define SERVER_H

#include "socket.h"
#include "player.h"
#include <map>

class Server
{
public:

                            Server(Socket*);
    Socket*                 getSocket();
    std::map<int, Player*>* getPlayers();

private:

    Socket*                 socket;
    std::map<int, Player*>  players;

};

#endif

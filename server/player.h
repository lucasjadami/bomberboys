#ifndef PLAYER_H
#define PLAYER_H

#include "socket.h"

class Player
{
public:

            Player(Socket*);
           ~Player();
   char*    getName();
   void     setName(char*);
   Socket*  getSocket();
   bool     isPlaying();

private:

    Socket* socket;
    char*   name;

};

#endif

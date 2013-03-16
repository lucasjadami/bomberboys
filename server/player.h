#ifndef PLAYER_H
#define PLAYER_H

#include "socket.h"

class Player
{
public:

            Player(Socket*, int, int);
           ~Player();
   char*    getName();
   void     setName(char*);
   Socket*  getSocket();
   bool     isPlaying();
   int      getX();
   int      getY();

private:

    Socket* socket;
    char*   name;
    int     x;
    int     y;

};

#endif

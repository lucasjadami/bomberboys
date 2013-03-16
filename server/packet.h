#ifndef PACKET_H
#define PACKET_H

#define NAME_SIZE 20

#define PACKET_LOGIN            0
#define PACKET_ADD_PLAYER       1
#define PACKET_REMOVE_PLAYER    2

#define PACKET_LOGIN_SIZE           NAME_SIZE
#define PACKET_ADD_PLAYER_SIZE      2 + NAME_SIZE
#define PACKET_REMOVE_PLAYER_SIZE   2

class Packet
{
public:

            Packet(int, char*);
           ~Packet();
    int     getId();
    char*   getData();

private:

    int     id;
    char*   data;

};

#endif
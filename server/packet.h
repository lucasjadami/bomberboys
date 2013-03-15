#ifndef PACKET_H
#define PACKET_H

#define PACKET_LOGIN 0

#define PACKET_LOGIN_SIZE 20

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

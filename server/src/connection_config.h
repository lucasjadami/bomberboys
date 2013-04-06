#ifndef CONNECTION_CONFIG_H
#define CONNECTION_CONFIG_H

//#define NON_BLOCKING_TCP_CONNECTION
//#define NON_BLOCKING_UDP_CONNECTION
#define BLOCKING_TCP_CONNECTION
//#define BLOCKING_UDP_CONNECTION

// Important note: in blocking modes, the sockets and thread maps don't have elements removed when a client
// disconnects. This is done to keep simplicity. But wait, this is not a good strategy if you want to run a game
// server!!! We did this because of lack of time and it is enough for our tests.

#if defined(BLOCKING_UDP_CONNECTION) || defined(BLOCKING_TCP_CONNECTION)
#define BLOCKING_MODE
#endif

#if defined(BLOCKING_UDP_CONNECTION) || defined(NON_BLOCKING_UDP_CONNECTION)
#define UDP_MODE
#else
#define TCP_MODE
#endif

#endif

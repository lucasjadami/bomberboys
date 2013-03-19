#include "non_blocking_tcp_connection.h"
#include "output.h"
#include "game.h"
#include <unistd.h>
#include <signal.h>
#include <cstdlib>

struct sigaction    sigIntHandler;
Connection* 	    connection;
Game*               game;

void exitHandler(int s)
{
	info("Server closed");
    delete game;
    delete connection;
	exit(0);
}

void connectionHandler(int eventId, Socket* socket)
{
    game->connectionHandler(eventId, socket);
}

#ifndef TESTBED
int main(int argc, char *argv[])
{
	sigIntHandler.sa_handler = exitHandler;
   	sigemptyset(&sigIntHandler.sa_mask);
   	sigIntHandler.sa_flags = 0;
   	sigaction(SIGINT, &sigIntHandler, NULL);

    info("Bomberboys server 1.0");

    int port = 10011;
	connection = new NonBlockingTcpConnection(&connectionHandler);
	connection->init(port);

	info("Server connection stabilished at port %d", port);

	game = new Game();
	game->createWorld();

	info("World created");

	while (true)
	{
		usleep(1000);
		connection->process();
		game->update();
	}

	return 0;
}
#endif

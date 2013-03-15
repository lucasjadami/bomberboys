#include "non_blocking_tcp_connection.h"
#include <unistd.h>
#include <signal.h>
#include <cstdlib>

struct sigaction 	sigIntHandler;
Connection* 		connection;

void exitHandler(int s)
{
	delete connection;
	exit(0);
}

int main(int argc, char *argv[])
{
	sigIntHandler.sa_handler = exitHandler;
   	sigemptyset(&sigIntHandler.sa_mask);
   	sigIntHandler.sa_flags = 0;
   	sigaction(SIGINT, &sigIntHandler, NULL);

	connection = new NonBlockingTcpConnection();
	connection->create();
	connection->init(10011);

	while (true)
	{
		usleep(1000);
		connection->process();
	}

	return 0;
}

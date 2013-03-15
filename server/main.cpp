#include "non_blocking_tcp_connection.h"
#include "output.h"
#include <unistd.h>
#include <signal.h>
#include <cstdlib>

struct sigaction 	sigIntHandler;
Connection* 		connection;

void exitHandler(int s)
{
	info("Server closed");

    delete connection;
	exit(0);
}

int main(int argc, char *argv[])
{
	sigIntHandler.sa_handler = exitHandler;
   	sigemptyset(&sigIntHandler.sa_mask);
   	sigIntHandler.sa_flags = 0;
   	sigaction(SIGINT, &sigIntHandler, NULL);

    info("Bomberboys server 1.0");

    int port = 10011;
	connection = new NonBlockingTcpConnection();
	connection->init(port);

	info("Server connection stabilished at port %d", port);

	while (true)
	{
		usleep(1000);
		connection->process();
	}

	return 0;
}

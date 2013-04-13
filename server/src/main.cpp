#include "non_blocking_tcp_connection.h"
#include "non_blocking_udp_connection.h"
#include "blocking_tcp_connection.h"
#include "blocking_udp_connection.h"
#include "output.h"
#include "game.h"

#ifdef TESTBED
#include "testbed/testbed.h"
#include "testbed/debug_draw.h"
#endif

#include <unistd.h>
#include <signal.h>
#include <cstdlib>

struct sigaction    sigIntHandler;
Connection* 	    connection;
Game*               game;
bool                gameRunning = true;

#ifdef TESTBED
DebugDraw           debugDraw;
b2Profile           maxProfile;
b2Profile           totalProfile;
int                 stepCount;
#endif

void exitHandler(int s)
{
	info("Server closed");
	gameRunning = false;

#ifdef TESTBED
    delete connection;
    delete game;
    exit(0);
#endif
}

void connectionHandler(int eventId, Socket* socket)
{
    game->connectionHandler(eventId, socket);
}

#ifdef TESTBED
void gameDrawHandler(Settings* settings)
{
    b2World* world = game->getWorld();

    b2Color c;
    b2AABB aabb;

    aabb.lowerBound = b2Vec2(0.0f, 0.0f);
    aabb.upperBound = b2Vec2(MAP_WIDTH, MAP_HEIGHT);
    c.Set(0.0f, 1.0f, 0.0f);
    debugDraw.DrawAABB(&aabb, c);

	world->DrawDebugData();

    c.Set(1.0f, 0.0f, 0.0f);
	b2Body* body = world->GetBodyList();
	while (body != NULL)
	{
	    debugDraw.DrawSegment(body->GetWorldCenter(), body->GetWorldCenter() + body->GetLinearVelocity(), c);
	    body = body->GetNext();
	}

    int textLine = 15;

	if (settings->drawStats)
	{
		int bodyCount = world->GetBodyCount();
		int contactCount = world->GetContactCount();
		int jointCount = world->GetJointCount();
		debugDraw.DrawString(5, textLine, "bodies/contacts/joints = %d/%d/%d", bodyCount, contactCount, jointCount);
		textLine += 15;

		int proxyCount = world->GetProxyCount();
		int height = world->GetTreeHeight();
		int balance = world->GetTreeBalance();
		float quality = world->GetTreeQuality();
		debugDraw.DrawString(5, textLine, "proxies/height/balance/quality = %d/%d/%d/%g", proxyCount, height, balance, quality);
		textLine += 15;
	}

	// Track maximum profile times.
	{
		const b2Profile& p = world->GetProfile();

		maxProfile.step = b2Max(maxProfile.step, p.step);
		maxProfile.collide = b2Max(maxProfile.collide, p.collide);
		maxProfile.solve = b2Max(maxProfile.solve, p.solve);
		maxProfile.solveInit = b2Max(maxProfile.solveInit, p.solveInit);
		maxProfile.solveVelocity = b2Max(maxProfile.solveVelocity, p.solveVelocity);
		maxProfile.solvePosition = b2Max(maxProfile.solvePosition, p.solvePosition);
		maxProfile.solveTOI = b2Max(maxProfile.solveTOI, p.solveTOI);
		maxProfile.broadphase = b2Max(maxProfile.broadphase, p.broadphase);

		totalProfile.step += p.step;
		totalProfile.collide += p.collide;
		totalProfile.solve += p.solve;
		totalProfile.solveInit += p.solveInit;
		totalProfile.solveVelocity += p.solveVelocity;
		totalProfile.solvePosition += p.solvePosition;
		totalProfile.solveTOI += p.solveTOI;
		totalProfile.broadphase += p.broadphase;
	}

	if (settings->drawProfile)
	{
		const b2Profile& p = world->GetProfile();

		b2Profile avgProfile;
		memset(&avgProfile, 0, sizeof(b2Profile));
		if (stepCount > 0)
		{
			float32 scale = 1.0f / stepCount;
			avgProfile.step = scale * totalProfile.step;
			avgProfile.collide = scale * totalProfile.collide;
			avgProfile.solve = scale * totalProfile.solve;
			avgProfile.solveInit = scale * totalProfile.solveInit;
			avgProfile.solveVelocity = scale * totalProfile.solveVelocity;
			avgProfile.solvePosition = scale * totalProfile.solvePosition;
			avgProfile.solveTOI = scale * totalProfile.solveTOI;
			avgProfile.broadphase = scale * totalProfile.broadphase;
		}

		debugDraw.DrawString(5, textLine, "step [avg] (max) = %5.2f [%6.2f] (%6.2f)", p.step, avgProfile.step, maxProfile.step);
		textLine += 15;
		debugDraw.DrawString(5, textLine, "collide [avg] (max) = %5.2f [%6.2f] (%6.2f)", p.collide, avgProfile.collide, maxProfile.collide);
		textLine += 15;
		debugDraw.DrawString(5, textLine, "solve [avg] (max) = %5.2f [%6.2f] (%6.2f)", p.solve, avgProfile.solve, maxProfile.solve);
		textLine += 15;
		debugDraw.DrawString(5, textLine, "solve init [avg] (max) = %5.2f [%6.2f] (%6.2f)", p.solveInit, avgProfile.solveInit, maxProfile.solveInit);
		textLine += 15;
		debugDraw.DrawString(5, textLine, "solve velocity [avg] (max) = %5.2f [%6.2f] (%6.2f)", p.solveVelocity, avgProfile.solveVelocity, maxProfile.solveVelocity);
		textLine += 15;
		debugDraw.DrawString(5, textLine, "solve position [avg] (max) = %5.2f [%6.2f] (%6.2f)", p.solvePosition, avgProfile.solvePosition, maxProfile.solvePosition);
		textLine += 15;
		debugDraw.DrawString(5, textLine, "solveTOI [avg] (max) = %5.2f [%6.2f] (%6.2f)", p.solveTOI, avgProfile.solveTOI, maxProfile.solveTOI);
		textLine += 15;
		debugDraw.DrawString(5, textLine, "broad-phase [avg] (max) = %5.2f [%6.2f] (%6.2f)", p.broadphase, avgProfile.broadphase, maxProfile.broadphase);
		textLine += 15;
	}
}

void gameUpdateHandler(Settings* settings)
{
    stepCount++;

    connection->process();

    b2World* world = game->getWorld();

    float32 timeStep = settings->hz > 0.0f ? 1.0f / settings->hz : float32(0.0f);

	uint32 flags = 0;
	flags += settings->drawShapes * b2Draw::e_shapeBit;
	flags += settings->drawJoints * b2Draw::e_jointBit;
	flags += settings->drawAABBs * b2Draw::e_aabbBit;
	flags += settings->drawPairs * b2Draw::e_pairBit;
	flags += settings->drawCOMs * b2Draw::e_centerOfMassBit;
	debugDraw.SetFlags(flags);

	world->SetWarmStarting(settings->enableWarmStarting > 0);
	world->SetContinuousPhysics(settings->enableContinuous > 0);
	world->SetSubStepping(settings->enableSubStepping > 0);

    game->update(timeStep, settings->velocityIterations, settings->positionIterations);
}
#endif

int main(int argc, char** argv)
{
	sigIntHandler.sa_handler = exitHandler;
   	sigemptyset(&sigIntHandler.sa_mask);
   	sigIntHandler.sa_flags = 0;
   	sigaction(SIGINT, &sigIntHandler, NULL);

    info("Bomberboys server 1.0");

#if defined(NON_BLOCKING_TCP_CONNECTION)
    info("Using non-blocking TCP connection");
	connection = new NonBlockingTcpConnection(&connectionHandler);
#elif defined(NON_BLOCKING_UDP_CONNECTION)
    info("Using non-blocking UDP connection");
	connection = new NonBlockingUdpConnection(&connectionHandler);
#elif defined(BLOCKING_TCP_CONNECTION)
    info("Using blocking TCP connection");
    connection = new BlockingTcpConnection(&connectionHandler);
#elif defined(BLOCKING_UDP_CONNECTION)
    info("Using blocking UDP connection");
    connection = new BlockingUdpConnection(&connectionHandler);
#else
    error("Connection type unknown");
#endif

    int port = 10011;
	connection->init(port);

	info("Server connection stabilished at port %d", port);

	game = new Game();
	game->createWorld();

	info("World created");

#ifndef TESTBED
	while (gameRunning)
	{
		usleep(1000);
		connection->process();
		game->update(1.0f / 60.0f, 8, 3);
	}
#else
    game->getWorld()->SetDebugDraw(&debugDraw);
    memset(&maxProfile, 0, sizeof(b2Profile));
    memset(&totalProfile, 0, sizeof(b2Profile));

    launchTestbed(gameUpdateHandler, gameDrawHandler, argc, argv, MAP_WIDTH, MAP_HEIGHT);
#endif

    delete connection;
    delete game;

	return 0;
}
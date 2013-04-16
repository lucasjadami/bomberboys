#!/bin/sh

ALIAS=server-trash0-tcp-blocking
CLIENT="./client-trash0-tcp.sh"
./server.sh $ALIAS $CLIENT


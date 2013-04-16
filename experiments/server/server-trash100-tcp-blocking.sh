#!/bin/sh

ALIAS=server-trash100-tcp-blocking
CLIENT="./client-trash100-tcp.sh"
./server.sh $ALIAS $CLIENT


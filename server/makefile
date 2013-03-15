# Makefile. 
#
# Use 'make' for general debug version.
# Use 'make release' for release version.
# See below the flags for each target.
#

FLAGS=
CC=g++

all: 		debug

clean: 		
		rm -rf *.o server

debug: 		FLAGS += -g
release:	FLAGS += -s -O3

debug: 		build
release: 	build

build: 		main.o output.o connection.o
		$(CC) $(FLAGS) main.o output.o connection.o -o server 

main.o: 	main.cpp
		$(CC) -c $(FLAGS) main.cpp
		
output.o:	output.cpp
		$(CC) -c $(FLAGS) output.cpp
	
connection.o: 	connection.cpp connection.h non_blocking_tcp_connection.cpp non_blocking_tcp_connection.h
		$(CC) -c $(FLAGS) connection.cpp non_blocking_tcp_connection.cpp


#ifndef MP_PCH_H
#define MP_PCH_H

// Standard includes.
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

// Networking
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <fcntl.h>

// Libraries
#include <ncurses.h>

// Some constants
#define TRUE 1
#define FALSE 0
#define FAIL 0
#define SOCKET int
#define PORT 39992

#endif

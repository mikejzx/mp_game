#ifndef MP_PCH_H
#define MP_PCH_H

// Standard includes.
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>

// Networking
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <fcntl.h>

// Other defines
#define TRUE 1
#define FALSE 0
#define FAIL 0
#define SOCKET int
#define PORT 39992

// Local includes
#include "comm/mp_packet.h"
#include "comm/mp_ostream.h"
#include "comm/mp_istream.h"

#endif

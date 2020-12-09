#ifndef MP_CLIENT_H
#define MP_CLIENT_H

/*
 * Structure containing info
 * about a client.
 */
typedef struct mp_client
{
	// Whether this client has been initialised.
	int initialised;

	// Index of this client in main player list.
	int index;

	// The socket connection.
	SOCKET sock;

	// Pointer to this client's thread.
	pthread_t thr;

	// Set to non-zero to stop the thread.
	volatile int thr_running;

	// I/O streams for this client.
	mp_istream* is;
	mp_ostream* os;

	// Player information
	int x, y;
} mp_client;

void client_init(mp_client* const, SOCKET);
void client_set_index(mp_client* const, int);
void client_deinit(mp_client* const);
void client_start(mp_client* const);
void* client_worker(void*);

#endif

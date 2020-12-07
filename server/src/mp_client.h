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

	// The socket connection.
	SOCKET sock;

	// Pointer to this client's thread.
	pthread_t thr;

	// Set to non-zero to stop the thread.
	volatile int thr_running;
} mp_client;

void client_init(mp_client* const, SOCKET);
void client_deinit(mp_client* const);

#endif

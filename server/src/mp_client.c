/*
 * mp_client.c
 *
 * Functions for working with client structures.
 */

#include "pch.h"
#include "mp_client.h"

/*
 * Initialise a client.
 *
 * @param c     Pointer to client to initialise.
 * @param sock  Client's socket.
 */
void client_init(mp_client* const c, SOCKET sock)
{
	// Set main members.
	c->initialised = TRUE;
	c->sock = sock;

	// Start the client's thread.
	c->thr_running = TRUE;
	int status = pthread_create(&c->thr, 0, client_worker, (void*)c);
	if (status != 0)
	{
		printf("Failed to create client thread\n");
		c->thr_running = FALSE;
		return;
	}

	// Detach thread.
	pthread_detach(c->thr);
}

/*
 * De-initialise a client.
 *
 * @param c  Pointer to client to deinitialise.
 */
void client_deinit(mp_client* const c)
{
	// Tell thread to stop and join.
	printf("Deinitialising client\n");
	c->thr_running = 0;
	pthread_join(c->thr, 0);
}

/*
 * Client worker thread
 */
void* client_worker(void* arg)
{
	// Cast argument back to a client struct.
	mp_client* const c = (mp_client* const)arg;

	printf("Started client worker thread.\n");

	// Run until we get signalled to stop.
	while (c->thr_running)
	{
		// Do stuff.
		// (Block until we get a packet, then process it here)
	}

	printf("Exiting client worker thread.\n");
	pthread_exit(NULL);
	return 0;
}

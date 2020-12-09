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
	// Set main members. (We set to initialised after
	// all these initialisations)
	c->initialised = FALSE;
	c->thr_running = FALSE;
	c->sock = sock;

	// Initialise I/O streams.
	if (!(c->os = ostream_new(sock)))
	{
		return;
	}

	c->initialised = TRUE;
}

/*
 * Actually start the client's worker thread.
 *
 * @param c  Pointer to client to start.
 */
void client_start(mp_client* const c)
{
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
	if (c->thr_running)
	{
		pthread_join(c->thr, 0);
		c->thr_running = FALSE;
	}

	// De-allocate everything.
	ostream_free(c->os);
	//istream_free(c->is);

	c->initialised = FALSE;
}

/*
 * Client worker thread
 */
void* client_worker(void* arg)
{
	// Cast argument back to a client struct.
	mp_client* const c = (mp_client* const)arg;

	printf("Started client worker thread.\n");

	// Send a hello packet to client, telling them that they're in.
	// and telling them
	{
		// ostream_begin(c->os, HELLO);
		// ostream_flush(c->os);
	}

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

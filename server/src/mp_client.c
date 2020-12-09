/*
 * mp_client.c
 *
 * Functions for working with client structures.
 */

#include "pch.h"
#include "mp_client.h"

// Forward declarations of externals that we reference.
extern unsigned g_max_players;
extern unsigned g_map_wid;
extern unsigned g_map_hei;
extern unsigned server_player_count(void);

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
		printf("Failed to allocate ostream for client!");
		return;
	}
	if (!(c->is = istream_new(sock)))
	{
		printf("Failed to allocate istream for client!");
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
	istream_free(c->is);

	// Close socket.
	close(c->sock);
	c->sock = 0;

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
	{
		ostream_begin(c->os, P_HELLO);

		// Player counts (cur, max)
		owrite_u16(c->os, (unsigned short)server_player_count());
		owrite_u16(c->os, (unsigned short)g_max_players);

		// Map width/height
		owrite_u8(c->os, (unsigned char)g_map_wid);
		owrite_u8(c->os, (unsigned char)g_map_hei);

		// Generate a random spawn position
		owrite_u8(c->os, (unsigned char)(rand() % g_map_wid));
		owrite_u8(c->os, (unsigned char)(rand() % g_map_hei));

		ostream_flush(c->os);
	}

	// Run until we get signalled to stop.
	while (c->thr_running)
	{
		// Block until we get a packet, then process it
		enum mp_packet packet = iread_begin(c->is);
		switch(packet)
		{
			// Client is disconnecting.
			case P_DISCONN:
			{
				// Stop the worker thread.
				goto worker_exit;
			}

			default:
			{
				// Unknown packet?
				printf("Ignoring unimplemented packet with code %d...", packet);
			}
		}
	}

worker_exit:
	printf("Exiting client worker thread.\n");

	// We can deinitialise the client itself here if not already done.
	c->thr_running = FALSE;
	client_deinit(c);

	pthread_exit(NULL);
	return 0;
}

/*
 * main.c
 *
 * Main translation unit of the server.
 *
 * This is the simple server-side application.
 */

#include "pch.h"
#include "mp_tcp.h"
#include "mp_client.h"

// For signal interupt handler.
static volatile sig_atomic_t signal_interrupt_caught = 0;
void signal_interrupt_handler(int param)
{
	(void)param;
	signal_interrupt_caught = 1;
}

// Variables
static mp_tcp* tcp;
static mp_client* clients;
static const unsigned MAX_PLAYERS = 2;

// Function prototypes.
int recv_loop(void);

/*
 * Entry point of the program.
 *
 * @return status. 0 on normal termination.
 */
int main(void)
{
	int status = 0;
	printf("-- Simple Game Server --\n");

	// Register signal interrupt handler.
	struct sigaction sigact_inter;
	sigact_inter.sa_handler = signal_interrupt_handler;
	sigaction(SIGINT, &sigact_inter, NULL);

	// Allocate TCP struct.
	if (!(tcp = tcp_new()))
	{
		printf("Error initialising TCP connection!\n");
		return -1;
	}
	printf("TCP listener initialised. Listening...\n");

	// Allocate memory for all the clients we will have.
	size_t clients_size = sizeof(mp_client) * MAX_PLAYERS;
	if (!(clients = malloc(clients_size)))
	{
		printf("Failed to allocate memory for clients.");
		status = -1;
		goto fail;
	}
	memset(clients, 0, clients_size);

	// Start receiving.
	while (recv_loop() && !signal_interrupt_caught);

	if (signal_interrupt_caught)
	{
		printf("Signal interrupt caught. Terminating...\n");
	}

fail:
	// Free memory
	tcp_free(tcp);
	if (clients)
	{
		for (unsigned i = 0; i < MAX_PLAYERS; ++i)
		{
			// Deinitialise each client before freeing all them.
			// Need to do this to stop threads.
			if (clients[i].initialised)
			{
				client_deinit(&clients[i]);
			}
		}
		// Now free the array.
		free(clients);
	}

	return status;
}

/*
 * Main receiver loop.
 *
 * @return TRUE if loop should continue.
 */
int recv_loop(void)
{
	// Accept incoming connections
	SOCKET csock = accept(tcp->handle, (struct sockaddr*)&tcp->addr, &tcp->addr_len);
	if (csock <= 0)
	{
		// Failed to accept connection.
		// Just continue listening.
		return TRUE;
	}
	printf("Accepted client connection request.\n");

	// Now we initialise our client. Memory for it was
	// allocated already on server startup.

	// Initialise client and their thread.
	mp_client tmp;
	client_init(&tmp, csock);

	// Look for an empty slot to store the client.
	int slot = -1;
	for (unsigned i = 0; i < MAX_PLAYERS; ++i)
	{
		if (!clients[i].initialised)
		{
			slot = i;
			break;
		}
	}
	if (slot == -1)
	{
		// Server is full. Send the SERVER_FULL error
		// code back to client, and close their connection.
		ostream_begin(tmp.os, P_ERROR);
		owrite_err(tmp.os, ERR_SERVER_FULL);
		ostream_flush(tmp.os);
		client_deinit(&tmp);
		close(csock);
		return TRUE;
	}

	// We have a slot, so copy the
	// old structure into this.
	clients[slot] = tmp;

	// All is good, we can actually start the client's worker thread.
	client_start(&clients[slot]);

	return TRUE;
}

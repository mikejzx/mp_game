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
static const int MAX_PLAYERS = 2;

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
	clients = malloc(clients_size);
	if (!clients)
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
	if (clients) { free(clients); }

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
	if (csock > 0)
	{
		printf("Accepted client connection request.\n");

		// Now we initialise our client. Memory for it was
		// allocated already on server startup.

		// Look for an empty slot.
		int slot = -1;
		for (unsigned i; i < MAX_PLAYERS; ++i)
		{
			if (clients[i].initialised)
			{
				continue;
			}
			slot = i;
			break;
		}
		if (slot == -1)
		{
			// Server is full. Respond to client,
			// then discard their connection.
			close(csock);
			printf("Closing client connection, as server is full.\n");
			return TRUE;
		}

		// Found a slot. Initialise client and their thread.
		client_init(&clients[slot], csock);
	}

	return TRUE;
}

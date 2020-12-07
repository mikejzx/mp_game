/*
 * mp_tcp.c
 *
 * Provides functions for dealing with TCP.
 */
#include "pch.h"
#include "mp_tcp.h"

/*
 * Allocate new TCP socket.
 *
 * @return pointer to socket that was allocated. FAIL on failure.
 */
mp_tcp* tcp_new(void)
{
	// Allocate
	mp_tcp* tcp = malloc(sizeof(mp_tcp));
	if (!tcp)
	{
		printf("Failed to allocate memory for TCP socket!\n");
		return FAIL;
	}
	memset(tcp, 0, sizeof(mp_tcp));

	// Create TCP socket file descriptor.
	if (!(tcp->handle = socket(AF_INET, SOCK_STREAM, 0)))
	{
		printf("Failed to create socket file descriptor.\n");
		goto fail;
	}

	// Attach socket to port.
	int opt = 1;
	if (setsockopt(tcp->handle, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, (const char*)&opt, sizeof(opt)))
	{
		printf("Failed to attach TCP socket to port.\n");
		goto fail;
	}

	// Set attributes.
	tcp->addr.sin_family = AF_INET;
	tcp->addr.sin_addr.s_addr = INADDR_ANY;
	tcp->addr.sin_port = htons((unsigned short)PORT);
	tcp->addr_len = sizeof(struct sockaddr_in);

	// Bind to port.
	if (bind(tcp->handle, (struct sockaddr*)&tcp->addr, tcp->addr_len) < 0)
	{
		printf("Failed to bind TCP socket\n");
		goto fail;
	}

	// Set socket to accept connections (backlog of 3)
	if (listen(tcp->handle, 3) < 0)
	{
		printf("Failed mark TCP socket as accepting connections.\n");
		goto fail;
	}

	// Set to non-blocking mode
	int nonblock = 1;
	if (fcntl(tcp->handle, F_SETFL, O_NONBLOCK, nonblock) < 0)
	{
		printf("Failed to set TCP socket as non-blocking\n");
		goto fail;
	}

	// Normal return
	return tcp;

	// Use this label for fails after the allocation.
fail:
	free(tcp);
	return FAIL;
}

/*
 * Free TCP socket.
 *
 * @param tcp  Socket to free.
 */
void tcp_free(mp_tcp* const tcp)
{
	// Close handle.
	if (tcp->handle)
	{
		close(tcp->handle);
	}

	// Free memory
	free(tcp);
}

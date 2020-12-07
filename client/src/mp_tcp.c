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
mp_tcp* tcp_new(char* const ip)
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

	// Set attributes.
	tcp->addr.sin_family = AF_INET;
	tcp->addr.sin_addr.s_addr = inet_addr(ip);
	tcp->addr.sin_port = htons((unsigned short)PORT);
	tcp->addr_len = sizeof(struct sockaddr_in);

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

/*
 * Connect the TCP socket.
 *
 * @param tcp  Socket to connect.
 *
 * @return FAIL if couldn't connect
 */
int tcp_connect(mp_tcp* const tcp)
{
	return !connect(tcp->handle, &tcp->addr, tcp->addr_len);
}


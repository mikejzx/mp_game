#ifndef MP_TCP_H
#define MP_TCP_H

/*
 * Structure containing data associated
 * with the main TCP connection.
 */
typedef struct mp_tcp
{
	// Listener handle
	SOCKET handle;

	// Address structure
	struct sockaddr_in addr;

	// Size of address structure.
	socklen_t addr_len;
} mp_tcp;

// Allocation methods
mp_tcp* tcp_new(char* const);
void tcp_free(mp_tcp* const);

// Other functions
int tcp_connect(mp_tcp* const);

#endif

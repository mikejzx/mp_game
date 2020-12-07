#ifndef MP_PACKETS_H
#define MP_PACKETS_H

/*
 * List of packets control codes used
 * in the project.
 */
enum mp_packet
{
	UNKNOWN = 0,
	HELLO   = 1, // Connect client to server.
	DISCONN = 2, // Disconnect from server.
};

/*
 * Packet error codes.
 */
enum mp_packet_err
{
	SUCCESS = 0,
	TIMED_OUT = 1,
	SERVER_FULL = 2,

	INTERNAL_ERROR = 255,
};

#endif

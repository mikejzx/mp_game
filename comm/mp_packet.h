#ifndef MP_PACKETS_H
#define MP_PACKETS_H

/*
 * List of packets control codes used
 * in the project.
 */
enum mp_packet
{
	// Unknown packet.
	P_UNKNOWN = 0,

	/*
	 * Server: there was an error.
	 * + next byte is the error code.
	 */
	P_ERROR   = 1,

	/*
	 * Server: client's request to join is accepted.
	 * to join the server.
	 */
	P_HELLO   = 1, // Connect client to server.

	/*
	 * Client: disconnected from server.
	 */
	P_DISCONN = 2, // Disconnect from server.
};

/*
 * Packet error codes.
 */
enum mp_packet_err
{
	ERR_SUCCESS = 0,
	ERR_TIMED_OUT = 1,
	ERR_SERVER_FULL = 2,

	ERR_INTERNAL = 255,
};

#endif

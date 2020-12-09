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
	 * [0, u8] error code.
	 */
	P_ERROR   = 1,

	/*
	 * Server: client's request to join is accepted.
	 * to join the server.
	 * + [0, u16] number of players in server.
	 * + [0, u16] maximum player count.
	 * + [0, u8] map width
	 * + [0, u8] map height
	 * + [0, u8] player's X spawn position
	 * + [0, u8] player's Y spawn position
	 */
	P_HELLO   = 2, // Connect client to server.

	/*
	 * Client: disconnected from server.
	 */
	P_DISCONN = 3, // Disconnect from server.
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

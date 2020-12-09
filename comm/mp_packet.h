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
	 * + [u8] error code.
	 */
	P_ERROR   = 1,

	/*
	 * Server: client's request to join is accepted.
	 * to join the server.
	 * + [u8] number of players in server.
	 * + [u8] maximum player count.
	 * + [u8] index of client in terms of the player list.
	 * + [u8] map width
	 * + [u8] map height
	 * + An array of all the players, with following data
	 *   for each:
	 *   - [u8] player's index
	 *   - [u8] player's X spawn position
	 *   - [u8] player's Y spawn position
	 */
	P_HELLO   = 2, // Connect client to server.

	/*
	 * Client: disconnected from server.
	 */
	P_DISCONN = 3, // Disconnect from server.

	/*
	 * Client: Position changed.
	 * (needs to constantly be called to keep in sync)
	 * + [u8] this client's X
	 * + [u8] this client's Y
	 */
	P_POS_UPDATE = 4,

	/*
	 * Server: state update.
	 * + [u8] number of players that follow this byte.
	 * + An array of players that need update
	 *   in the following structure:
	 *   - [u8] player index
	 *   - [u8] x position
	 *   - [u8] y position
	 */
	P_UPDATE = 5,
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

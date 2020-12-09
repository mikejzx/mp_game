/*
 * mp_istream.c
 *
 * Implementation of methods for a simple input stream.
 */

#include "pch.h"
#include "mp_packet.h"
#include "mp_istream.h"

/*
 * Initialise a new input stream.
 *
 * @param sock  Socket stream will use
 *
 * @return the newly allocated stream.
 */
mp_istream* const istream_new(SOCKET sock)
{
	// Allocate the structure.
	mp_istream* i = malloc(sizeof(mp_istream));
	if (!i)
	{
		return 0;
	}

	// Only need this if we end up adding more
	// members to the structure.
	//memset(i, 0, sizeof(mp_istream));

	i->sock = sock;
	return i;
}

/*
 * Free an istream.
 *
 * @param i  Stream to free.
 */
void istream_free(mp_istream* const i)
{
	if (i) free(i);
}

/*
 * Begin a new packet. (Simply reads packet byte)
 *
 * @param i  The stream we are working with.
 *
 * @return the packet control code.
 */
enum mp_packet iread_begin(mp_istream* const i)
{
	return (enum mp_packet)iread_u8(i);
}

/* @return error code read from packet.  */
enum mp_packet_err iread_err(mp_istream* const i)
{
	return (enum mp_packet_err)iread_u8(i);
}


/* @return 8-bit unsigned from packet */
unsigned char iread_u8(mp_istream* const i)
{
	unsigned char x;
	TEMP_FAILURE_RETRY(recv(i->sock, &x, sizeof(x), 0));
	return x;
}

/* @return 16-bit unsigned from packet */
unsigned iread_u16(mp_istream* const i)
{
	// Read 2 bytes from packet
	char bytes[sizeof(unsigned short)];
	TEMP_FAILURE_RETRY(recv(i->sock, &bytes, sizeof(bytes), 0));

	// Reconstruct from little-endian order
	return (unsigned short)(
		(unsigned char)bytes[1] << 8 |
		(unsigned char)bytes[0]
	);
}

/* @return 32-bit unsigned from packet */
unsigned iread_u32(mp_istream* const i)
{
	// Read 4 bytes from packet
	char bytes[sizeof(unsigned)];
	TEMP_FAILURE_RETRY(recv(i->sock, &bytes, sizeof(bytes), 0));

	// Reconstruct from little-endian order
	return (unsigned)(
		(unsigned char)bytes[3] << 24 |
		(unsigned char)bytes[2] << 16 |
		(unsigned char)bytes[1] << 8 |
		(unsigned char)bytes[0]
	);
}

char* iread_str(mp_istream* const i, size_t* l)
{
	// Read length
	unsigned len = iread_u32(i);

	// Read bytes
	char* bytes = malloc(len + 1);
	TEMP_FAILURE_RETRY(recv(i->sock, bytes, len, 0));

	// Insert null-terminator.
	bytes[len] = '\0';

	if (l != 0) *l = len;
	return bytes;
}

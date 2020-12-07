/*
 * mp_ostream.c
 *
 * Implementation of methods for output stream.
 */

#include "pch.h"
#include "mp_packets.h"
#include "mp_ostream.h"

/*
 * Flush a stream. (i.e send data)
 *
 * @param o  Stream to flush.
 */
void flush(mp_ostream* const o)
{
	// Send the data. (TODO)
	// send(o->sock, DATA ???, SIZE ???, 0);

	// Clear the stream
	// TODO
}

/*
 * Begin a new packet.
 *
 * @param o  The stream we are working with.
 * @param p  Packet control code.
 *
 * @return the stream that is being handled.
 */
mp_ostream* const ostream_begin(mp_ostream* const o, const enum mp_packet p)
{
	// Write the packet byte.

	return o;
}

// Macro for write methods below.
// This simply writes bytes of x in L-E format.
#define WRITE_BYTES(o, x)\
	size_t len = sizeof(x);\
	unsigned char buf[len];\
	memcpy(buf, &x, len);\
	for (unsigned i = 0; i < len; ++i)\
	{\
		write_u8(o, buf[i]);\
	}

// Write methods.
mp_ostream* const write_u8(mp_ostream* const o, unsigned char x) { WRITE_BYTES(o, x); return o; }
mp_ostream* const write_u16(mp_ostream* const o, unsigned short x) { WRITE_BYTES(o, x); return o; }
mp_ostream* const write_u32(mp_ostream* const o, unsigned x) { WRITE_BYTES(o, x); return o; }
mp_ostream* const write_8(mp_ostream* const o, char x) { WRITE_BYTES(o, x); return o; }
mp_ostream* const write_16(mp_ostream* const o, short x) { WRITE_BYTES(o, x); return o; }
mp_ostream* const write_32(mp_ostream* const o, int x) { WRITE_BYTES(o, x); return o; }
mp_ostream* const write_str(mp_ostream* const o, size_t len, char* data)
{
	write_u32(o, (unsigned)len);
	for(unsigned i = 0; i < len; ++i)
	{
		write_8(o, data[i]);
	}
	return o;
}

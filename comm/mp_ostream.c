/*
 * mp_ostream.c
 *
 * Implementation of methods for output stream.
 */

#include "pch.h"
#include "mp_packet.h"
#include "mp_ostream.h"

/*
 * Initialise a new output stream with default
 * initial size.
 *
 * @param s  Socket stream will use.
 *
 * @return the newly allocated stream.
 */
mp_ostream* const ostream_new(SOCKET sock)
{
	return ostream_new_ex(sock, OSTREAM_INIT_BUF_SIZE);
}

/*
 * Initialise a new output stream.
 *
 * @param sock  Socket stream will use
 * @param size  Initial size of the buffer.
 *
 * @return the newly allocated stream.
 */
mp_ostream* const ostream_new_ex(SOCKET sock, unsigned size)
{
	// Allocate the struct.
	mp_ostream* o = malloc(sizeof(mp_ostream));
	if (!o)
	{
		return 0;
	}
	memset(o, 0, sizeof(mp_ostream));

	o->sock = sock;

	// Allocate the internal buffer
	o->buf_size = size * sizeof(unsigned char);
	o->buf_len = 0;
	if (!(o->buf = malloc(o->buf_size)))
	{
		return 0;
	}
	memset(o->buf, 0, o->buf_size);

	return o;
}

/*
 * Free an ostream.
 *
 * @param o  Stream to free.
 */
void ostream_free(mp_ostream* const o)
{
	if (!o) return;
	if (o->buf) free(o->buf);
	free(o);
}

/*
 * Begin a new packet. (Simply writes packet byte)
 *
 * @param o  The stream we are working with.
 * @param p  Packet control code.
 *
 * @return the stream that is being handled.
 */
mp_ostream* const ostream_begin(mp_ostream* const o, const enum mp_packet p)
{
	return owrite_u8(o, (unsigned char)p);
}

/*
 * Flush a stream. (i.e send data)
 *
 * @param o  Stream to flush.
 */
void ostream_flush(mp_ostream* const o)
{
	// printf("Sending %d bytes: < ", o->buf_len);
	// for (unsigned i = 0; i < o->buf_len; ++i)
	// {
	// 	printf("%02X ", o->buf[i]);
	// }
	// printf(">\n");

	// Send the data.
	send(o->sock, o->buf, o->buf_len, 0);

	// Clear the stream
	memset(o->buf, 0, o->buf_size);
	o->buf_len = 0;
}

// Macro for write methods below.
// This simply writes bytes of x in L-E format.
#define WRITE_BYTES(o, x)\
	size_t len = sizeof(x);\
	unsigned char buf[len];\
	memcpy(buf, &x, len);\
	for (unsigned i = 0; i < len; ++i)\
	{\
		owrite_u8(o, buf[i]);\
	}

/*
 * Write a single unsigned byte. This is the base
 * write method from which the rest are built on.
 */
mp_ostream* const owrite_u8(mp_ostream* const o, unsigned char x)
{
	// Reallocate if we need more space.
	size_t next_size = o->buf_len * sizeof(unsigned char) + sizeof(x);
	if (next_size > o->buf_size)
	{
		// We allocate double the size for now.
		o->buf_size = o->buf_size * 2 + 4;
		o->buf = realloc(o->buf, o->buf_size);
		// printf("REALLOCATED (to %d)\n", o->buf_size);
	}

	// Write the byte at current index.
	o->buf[o->buf_len++] = x;

	return o;
}
// Other write methods.
mp_ostream* const owrite_err(mp_ostream* const o, const enum mp_packet_err e)
{
	return owrite_u8(o, (unsigned char)e);
}
mp_ostream* const owrite_u16(mp_ostream* const o, unsigned short x) { WRITE_BYTES(o, x); return o; }
mp_ostream* const owrite_u32(mp_ostream* const o, unsigned x) { WRITE_BYTES(o, x); return o; }
mp_ostream* const owrite_8(mp_ostream* const o, char x) { WRITE_BYTES(o, x); return o; }
mp_ostream* const owrite_16(mp_ostream* const o, short x) { WRITE_BYTES(o, x); return o; }
mp_ostream* const owrite_32(mp_ostream* const o, int x) { WRITE_BYTES(o, x); return o; }
mp_ostream* const owrite_str(mp_ostream* const o, char* const data, size_t len)
{
	owrite_u32(o, (unsigned)len);
	for(unsigned i = 0; i < len; ++i)
	{
		owrite_8(o, data[i]);
	}
	return o;
}

#ifndef MP_OSTREAM_H
#define MP_OSTREAM_H

// Basic definitions we need.
#define TRUE 1
#define FALSE 0
#define SOCKET int

/*
 * This is a basic "output stream" that allows
 * us to write data to a socket in a fairly
 * organised fashion.
 *
 * Bytes are written in little-endian format.
 */
typedef struct mp_ostream
{
	// Socket
	SOCKET sock;

	// Data buffer.
	// ??? buf;
} mp_ostream;

// Stream functions
void flush(mp_ostream* const);
mp_ostream* const ostream_begin(mp_ostream* const, const enum mp_packet);

// Write functions
mp_ostream* const write_u8(mp_ostream* const, unsigned char);
mp_ostream* const write_u16(mp_ostream* const, unsigned short);
mp_ostream* const write_u32(mp_ostream* const, unsigned);
mp_ostream* const write_8(mp_ostream* const, char);
mp_ostream* const write_16(mp_ostream* const, short);
mp_ostream* const write_32(mp_ostream* const, int);
mp_ostream* const write_str(mp_ostream* const, size_t, char*);

#endif

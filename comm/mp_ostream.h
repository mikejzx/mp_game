#ifndef MP_OSTREAM_H
#define MP_OSTREAM_H

// Basic definitions we need.
#define TRUE 1
#define FALSE 0
#define SOCKET int
#define OSTREAM_INIT_BUF_SIZE 16

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
	unsigned char* buf;

	// Current occupied length of buffer
	unsigned buf_len;

	// Total storage size of buffer
	unsigned buf_size;
} mp_ostream;

// Allocation
mp_ostream* const ostream_new(SOCKET);
mp_ostream* const ostream_new_ex(SOCKET, unsigned);
void ostream_free(mp_ostream* const);

// Stream functions
mp_ostream* const ostream_begin(mp_ostream* const, const enum mp_packet);
void ostream_flush(mp_ostream* const);

// Write functions
mp_ostream* const owrite_err(mp_ostream* const, const enum mp_packet_err);
mp_ostream* const owrite_u8(mp_ostream* const, unsigned char);
mp_ostream* const owrite_u16(mp_ostream* const, unsigned short);
mp_ostream* const owrite_u32(mp_ostream* const, unsigned);
mp_ostream* const owrite_8(mp_ostream* const, char);
mp_ostream* const owrite_16(mp_ostream* const, short);
mp_ostream* const owrite_32(mp_ostream* const, int);
mp_ostream* const owrite_str(mp_ostream* const, char* const, size_t);

#endif

#ifndef MP_ISTREAM_H
#define MP_ISTREAM_H

// Basic definitions we need.
#define TRUE 1
#define FALSE 0
#define SOCKET int

/*
 * This is a basic "input stream" that
 * lets us read data from a socket.
 */
typedef struct mp_istream
{
	// Socket
	SOCKET sock;
} mp_istream;

// Allocation
mp_istream* const istream_new(SOCKET);
void istream_free(mp_istream* const);

// Read methods
enum mp_packet iread_begin(mp_istream* const);
enum mp_packet_err iread_err(mp_istream* const);
unsigned char iread_u8(mp_istream* const);
unsigned iread_u16(mp_istream* const);
unsigned iread_u32(mp_istream* const);
char* iread_str(mp_istream* const, size_t*);

#endif

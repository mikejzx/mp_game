/*
 * main.c
 *
 * Main translation unit of the project.
 *
 * The client is a simple "game" which allows a
 * player to simply move around in a grid.
 *
 * This is the proper version of the client,
 * that has networking support built-in.
 *
 * Basic plan of how to do things:
 * + (DONE) Ask user to enter IP address of server (and a username in future)
 * + (DONE) Connect to the server.
 * + Client asks server if they're in.
 * + Server decides where player spawns, sends data back.
 * + Client asks server for all the players information
 *   and client stores this information.
 * + Client continuously asks server where other players are.
 * + Client also continously tells server where they are.
 * + On exit, client tells server that they disconnected.
 */

#include "pch.h"
#include "mp_tcp.h"

// Debugging
#define DEBUG_SKIP_SERVER 1
#define DEBUG_SERVER_IP_OVERRIDE "127.0.0.1"

// Grid constants
#define GRID_WIDTH 40
#define GRID_HEIGHT 15

// Other constants
#define CURSOR_HIDE 0
#define CURSOR_SHOW 0
#define GAME_SPEED 300
#define IN_TERMINATE -1

// Tile chars
#define TILE_DEFAULT '.'
#define TILE_PLAYER 'X'

// Function prototypes
void draw_grid(void);
int get_input(void);

// For signal interupt handler.
static volatile sig_atomic_t signal_interrupt_caught = 0;
void signal_interrupt_handler(int param)
{
	(void)param;
	signal_interrupt_caught = 1;
}

// Structure for players in the game.
typedef struct
{
	// Whether this player is the actually player
	// on this computer.
	int is_player;

	// X/Y position.
	int x;
	int y;
} player;


// Main variables.
// static player* players;
// static size_t player_count;
// static const int player_idx;
static mp_tcp* tcp;
static mp_istream* is;
static mp_ostream* os;

/*
 * Entry point of the application.
 */
int main(void)
{
	// Status code.
	int status = 0;

	// Register signal interrupt handler.
	struct sigaction sigact_inter;
	sigact_inter.sa_handler = signal_interrupt_handler;
	sigaction(SIGINT, &sigact_inter, NULL);

	// Initialise general ncurses stuff
	initscr();   // Initialise main screen.
	timeout(-1); // No timeout.

#if DEBUG_SKIP_SERVER == 0
	// Prompt user for server IP address.
	const char* IP_TEMPLATE = "XXX.XXX.XXX.XXX";
	char serverip[strlen(IP_TEMPLATE)];
	{
		clear();

		// Create prompt in middle of screen.
		int row, col;
		getmaxyx(stdscr, row, col);
		const char prompt[] = "Enter server IP: ";
		const char title[] = "-- Simple Multiplayer Game --";
		mvaddstr(row / 2 - 1, (col - strlen(title)) / 2, title);
		mvaddstr(row / 2, (col - strlen(prompt) - strlen(IP_TEMPLATE)) / 2, prompt);

		// Get user input
		getstr(serverip);

		refresh();
	}
#else
	char serverip[] = DEBUG_SERVER_IP_OVERRIDE;
#endif

	// Set up our TCP socket.
	if (!(tcp = tcp_new(serverip)))
	{
		printf("Error initialising TCP connection!\n");
		status = -1;
		goto fail;
	}

	// Create our I/O streams.
	if (!(os = ostream_new(tcp->handle)))
	{
		printf("Failed to create output stream.\n");
		goto fail;
	}
	if (!(is = istream_new(tcp->handle)))
	{
		printf("Failed to create input stream.\n");
		goto fail;
	}

	// Try and connect to the server using IP they gave us.
	if (!tcp_connect(tcp))
	{
		printf("Could not establish connection with server.\n");
		goto fail;
	}

	// Read the server's response packet to our connection.
	// If we get a P_HELLO then we are good to go.
	{
		enum mp_packet res = iread_begin(is);
		if (res == P_ERROR)
		{
			// Got an error. Check what it is:
			enum mp_packet_err err = iread_err(is);
			switch (err)
			{
				case ERR_TIMED_OUT:
				{
					printf("Error: Timed out\n");
				} break;

				case ERR_SERVER_FULL:
				{
					printf("Error: Server full\n");
				} break;

				case ERR_INTERNAL:
				{
					printf("Error: Internal server error.\n");
				} break;

				default:
				{
					printf("Unknown error occurred.\n");
				} break;
			}
			goto fail;
		}
		else if (res != P_HELLO)
		{
			printf("Did not get P_HELLO response.\n");
			goto fail;
		}

		// We got a P_HELLO. Now read data that server sent.
		unsigned pcount = (unsigned)iread_u16(is);
		unsigned pmax =   (unsigned)iread_u16(is);
		unsigned mapwid = (unsigned)iread_u8(is);
		unsigned maphei = (unsigned)iread_u8(is);
		unsigned spawn_x = (unsigned)iread_u8(is);
		unsigned spawn_y = (unsigned)iread_u8(is);
		printf("There are %d/%d players in the server.\n", pcount, pmax);
		printf("Map size: %dx%d\n", mapwid, maphei);
		printf("Spawn pos: [%d, %d]\n", spawn_x, spawn_y);
	}

	// Wait until we send exit signal.
	while(!signal_interrupt_caught);

	// Setup ncurses for our actual game (non-blocking)
	// curs_set(CURSOR_HIDE); // Hide cursor
	// cbreak();              // One char at a time input
	// noecho();              // Don't echo input
	// timeout(GAME_SPEED);   // Non block.
	// keypad(stdscr, TRUE);  // Special keys (i.e arrows)

	// Game code here...

	// Tell server that we disconnected.
	ostream_begin(os, P_DISCONN);
	ostream_flush(os);

fail:
	// Free memory.
	if (os) { ostream_free(os); }
	if (is) { istream_free(is); }

	// Uninitialise ncurses
	curs_set(CURSOR_SHOW);
	endwin();

	return status;
}

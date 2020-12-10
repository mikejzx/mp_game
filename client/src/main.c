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
#define DEBUG_SKIP_SERVER 0
#define DEBUG_SERVER_IP_OVERRIDE "127.0.0.1"

// Other constants
#define CURSOR_HIDE 0
#define CURSOR_SHOW 0
#define GAME_SPEED 25

// Function prototypes
void draw_map(void);
int get_input(void);
int start_worker(void);
void stop_worker(void);
void* worker_func(void*);

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
	int x, y;

	// Player's index.
	int index;
} player;


// Main variables.
static player* players = 0;
static size_t player_count = 0;
static size_t max_players = 0;
static int player_idx = 0;
static int glob_player_idx = 0;
static unsigned map_width = 0;
static unsigned map_height = 0;
static mp_tcp* tcp;
static mp_istream* is;
static mp_ostream* os;
static pthread_t thr;
static int thr_running = FALSE;

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

		// If user entered 'localhost' replace it with 127.0.0.1
		if (strcmp(serverip, "localhost") == 0)
		{
			strcpy(serverip, "127.0.0.1");
		}

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
		player_count = (unsigned)iread_u8(is);
		max_players = (unsigned)iread_u8(is);
		glob_player_idx = (int)iread_u8(is);
		map_width = (unsigned)iread_u8(is);
		map_height = (unsigned)iread_u8(is);

		// Read players
		players = malloc(sizeof(player) * player_count);
		for (unsigned i = 0; i < player_count; ++i)
		{
			players[i].index = (int)iread_u8(is);
			players[i].x = (int)iread_u8(is);
			players[i].y = (int)iread_u8(is);

			if (players[i].index == glob_player_idx)
			{
				players[i].is_player = TRUE;
				player_idx = i;
			}
		}
	}

	// Set up the network thread.
	if (!start_worker())
	{
		printf("Failed to start the network thread.\n");
		goto fail;
	}

	// Setup ncurses for our actual game (non-blocking)
	curs_set(CURSOR_HIDE); // Hide cursor
	cbreak();              // One char at a time input
	noecho();              // Don't echo input
	timeout(GAME_SPEED);   // Non block.
	keypad(stdscr, TRUE);  // Special keys (i.e arrows)

	// Run game until we send exit signal.
	while(!signal_interrupt_caught)
	{
		// Draw map
		draw_map();

		// Get input, and exit if necessary
		if (!get_input())
		{
			break;
		}
	}

	if (signal_interrupt_caught)
	{
		printf("Caught interrupt signal. Terminating...");
	}

	// Wait for thread to stop.
	stop_worker();
	while(thr_running);

	// Tell server that we disconnected.
	ostream_begin(os, P_DISCONN);
	ostream_flush(os);

fail:
	// Uninitialise ncurses
	curs_set(CURSOR_SHOW);
	endwin();

	// Free memory.
	if (os) { ostream_free(os); }
	if (is) { istream_free(is); }
	if (players) { free(players); }

	return status;
}

/*
 * Draws the map with players, etc.
 */
void draw_map(void)
{
	// Clear old map
	clear();

	// Constants
	const int TILE_WID = 2;
	const char TILE_DEFAULT = '.';
	const char TILE_PLAYER = 'X';

	// Draw map from top, left-to-right
	for (unsigned y = 0; y < map_height; ++y)
	{
		for (unsigned x = 0; x < map_width; ++x)
		{
			// Draw tile
			mvaddch(y, TILE_WID * x, TILE_DEFAULT);

			// Draw space
			mvaddch(y, TILE_WID * x + 1, ' ');
		}
	}

	// Draw players
	for (unsigned p = 0; p < player_count; ++p)
	{
		mvaddch(players[p].y, players[p].x * TILE_WID, TILE_PLAYER);
	}

	// Tell ncurses to redraw
	refresh();
}

/*
 * Gets player input.
 *
 * @return 0 when user wants to exit game.
 */
int get_input(void)
{
	int ch;
	switch (ch = getch())
	{
		// Quit
		case ('Q'):
		case ('q'):
		{
			return FALSE;
		} break;

		// Move left
		case ('A'):
		case ('a'):
		case ('H'):
		case ('h'):
		case (KEY_LEFT):
		{
			// Move player position 1 left.
			players[player_idx].x = (players[player_idx].x - 1) % (int)map_width;
		} break;

		// Move right
		case ('D'):
		case ('d'):
		case ('L'):
		case ('l'):
		case (KEY_RIGHT):
		{
			// Move player position 1 right.
			players[player_idx].x = (players[player_idx].x + 1) % (int)map_width;
		} break;

		// Move up
		case ('W'):
		case ('w'):
		case ('K'):
		case ('k'):
		case (KEY_UP):
		{
			// Move player position 1 up.
			players[player_idx].y = (players[player_idx].y - 1) % (int)map_height;
		} break;

		// Move down
		case ('S'):
		case ('s'):
		case ('J'):
		case ('j'):
		case (KEY_DOWN):
		{
			// Move player position 1 down.
			players[player_idx].y = (players[player_idx].y + 1) % (int)map_height;
		} break;
	}

	// Wrap for below zero.
	if (players[player_idx].x < 0)
	{
		players[player_idx].x = map_width - 1;
	}
	if (players[player_idx].y < 0)
	{
		players[player_idx].y = map_height - 1;
	}

	return TRUE;
}

/*
 * Starts the network worker thread.
 */
int start_worker(void)
{
	// Create and start thread.
	thr_running = TRUE;
	if (pthread_create(&thr, 0, worker_func, 0) != 0)
	{
		thr_running = FALSE;
		return FALSE;
	}

	// Detach thread.
	pthread_detach(thr);

	return TRUE;
}

/*
 * Stops the network worker thread.
 */
void stop_worker(void)
{
	if (thr_running)
	{
		thr_running = FALSE;
		pthread_join(thr, 0);
	}
}

/*
 * Actual worker method
 */
void* worker_func(void* arg)
{
	// Network loop
	while(thr_running && !signal_interrupt_caught)
	{
		// Here we constantly send our position to the server.
		// The server will respond with the positions of all
		// the players in the game.
		ostream_begin(os, P_POS_UPDATE);
		owrite_u8(os, (unsigned char)players[player_idx].x);
		owrite_u8(os, (unsigned char)players[player_idx].y);
		ostream_flush(os);

		// On success the server will respond with P_UPDATE
		enum mp_packet res = iread_begin(is);
		switch (res)
		{
			case P_UPDATE:
			{
				// Normal update. Server responds with the
				// current server state.
				unsigned pcount = (unsigned)iread_u8(is);
				if (player_count != pcount)
				{
					players = realloc(players, pcount * sizeof(player));
					player_count = pcount;
				}

				for (unsigned i = 0; i < player_count; ++i)
				{
					// Read this player.
					int idx = (int)iread_u8(is);
					int x = (int)iread_u8(is);
					int y = (int)iread_u8(is);

					if (idx == glob_player_idx)
					{
						player_idx = i;
					}
					else
					{
						players[i].index = idx;
						players[i].x = x;
						players[i].y = y;
					}
				}
			} break;

			case P_ERROR:
			{
				// An error occurred.
				enum mp_packet_err err = iread_err(is);
				(void)err;
			} break;

			default:
			{
				break;
			}
		}
	}

	// Exit thread.
	thr_running = FALSE;
	pthread_exit(NULL);
	return 0;
}

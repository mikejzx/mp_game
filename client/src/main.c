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
 * + Ask user to enter IP address of server
 *   (and a username in future)
 * + Connect to the server.
 * + Server decides where player spawns, sends data back.
 * + Client asks server for all the players information
 *   and client stores this information.
 * + Client continuously asks server where other players are.
 * + Client also continously tells server where they are.
 * + On exit, client tells server that they disconnected.
 */

#include "pch.h"

// Debugging
#define DEBUG_SKIP_SERVER 1
#define DEBUG_SERVER_IP_OVERRIDE "localhost"

// Grid constants
#define GRID_WIDTH 40
#define GRID_HEIGHT 15

// Other constants
#define TRUE 1
#define FALSE 0
#define FAIL 0
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

/*
 * Entry point of the application.
 */
int main(void)
{
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

	// Try and connect to the server that user gave.


	// Wait until we send exit signal.
	while(!signal_interrupt_caught);

	// Setup ncurses for our actual game (non-blocking)
	// curs_set(CURSOR_HIDE); // Hide cursor
	// cbreak();              // One char at a time input
	// noecho();              // Don't echo input
	// timeout(GAME_SPEED);   // Non block.
	// keypad(stdscr, TRUE);  // Special keys (i.e arrows)

	// Uninitialise ncurses
	curs_set(CURSOR_SHOW);
	endwin();

	return 0;
}

/*
 * main.c
 *
 * Main translation unit of the project.
 *
 * The client is a simple "game" which allows a
 * player to simply move around in a grid.
 */

#include "pch.h"

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

// Inputs
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
static player* players;
static size_t player_count;
static const int player_idx;

/*
 * Entry point of the application.
 */
int main(void)
{
	// Register signal interrupt handler.
	struct sigaction sigact_inter;
	sigact_inter.sa_handler = signal_interrupt_handler;
	sigaction(SIGINT, &sigact_inter, NULL);

	// Allocate for all players in the game.
	player_count = 1;
	players = malloc(sizeof(player) * player_count);
	players[0].is_player = TRUE;
	players[0].x = 1;
	players[0].y = 2;

	// Initialise ncurses
	initscr();             // Initialise main screen.
	cbreak();              // One char at a time input
	timeout(GAME_SPEED);   // Non block.
	noecho();              // Don't echo input
	keypad(stdscr, TRUE);  // Special keys (i.e arrows)
	curs_set(CURSOR_HIDE); // Hide cursor

	// Main game loop. Run until we get a interupt signal.
	while (!signal_interrupt_caught)
	{
		// Draw the board.
		draw_grid();

		// Get input
		// Break if necessary.
		if (get_input() == IN_TERMINATE)
		{
			break;
		}
	}

	if (signal_interrupt_caught)
	{
		printf("Signal interrupt caught. Terminating...");
	}

	// Deinitialise ncurses
	curs_set(CURSOR_SHOW);
	endwin();

	free(players);

	return 0;
}

/*
 * Draws the grid, with player, etc.
 */
void draw_grid(void)
{
	// Clear old grid.
	clear();

	const int TILE_WID = 2;

	// Draw rows
	for (unsigned y = 0; y < GRID_HEIGHT; ++y)
	{
		// Draw columns
		for (unsigned x = 0; x < GRID_WIDTH; ++x)
		{
			// Draw tile.
			mvwaddch(stdscr, y, TILE_WID * x, TILE_DEFAULT);

			// Draw a space.
			mvwaddch(stdscr, y, TILE_WID * x + 1, ' ');
		}
	}

	// Draw playres
	for (unsigned p = 0; p < player_count; ++p)
	{
		mvwaddch(stdscr, players[p].y, TILE_WID * players[p].x, TILE_PLAYER);
	}

	refresh();
}

/*
 * Gets user input
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
			return IN_TERMINATE;
		} break;

		// Move left
		case ('A'):
		case ('a'):
		case (KEY_LEFT):
		{
			// Move player position 1 left.
			players[player_idx].x = (players[player_idx].x - 1) % GRID_WIDTH;
		} break;

		// Move right
		case ('D'):
		case ('d'):
		case (KEY_RIGHT):
		{
			// Move player position 1 right.
			players[player_idx].x = (players[player_idx].x + 1) % GRID_WIDTH;
		} break;

		// Move up
		case ('W'):
		case ('w'):
		case (KEY_UP):
		{
			// Move player position 1 up.
			players[player_idx].y = (players[player_idx].y - 1) % GRID_HEIGHT;
		} break;

		// Move down
		case ('S'):
		case ('s'):
		case (KEY_DOWN):
		{
			// Move player position 1 down.
			players[player_idx].y = (players[player_idx].y + 1) % GRID_HEIGHT;
		} break;
	}

	// Wrap for below zero.
	if (players[player_idx].x < 0)
	{
		players[player_idx].x = GRID_WIDTH - 1;
	}
	if (players[player_idx].y < 0)
	{
		players[player_idx].y = GRID_HEIGHT - 1;
	}

	return 0;
}

/**
 * @file main.c
 * @brief Entry point for Thimlich Hunt game.
 *
 * This file initializes the Raylib window and runs the main game loop.
 */

#include "raylib.h"
#include "game.h"

/**
 * main - Entry point of the application
 *
 * Return: 0 on successful execution
 */
int main(void)
{
	const int screen_width = 1280;
	const int screen_height = 720;

	/* Initialize window */
	InitWindow(screen_width, screen_height, "Thimlich Hunt");

	/* Set target frames per second */
	SetTargetFPS(60);

	/**
	 * Main game loop
	 * Runs continuously until the user closes the window
	 */
	/* Run the game logic */
    game_run();


	/* Close the window and clean up resources */
	CloseWindow();

	return (0);
}
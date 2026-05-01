/**
 * @file main.c
 * @brief Entry point for Thimlich Hunt game.
 *
 * This file initializes the Raylib window and runs the main game loop.
 */

#include "raylib.h"

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
	while (!WindowShouldClose())
	{
		/* Begin drawing frame */
		BeginDrawing();

		/* Clear the background to black */
		ClearBackground(BLACK);

		/* Render basic text to confirm rendering works */
		DrawText("Thimlich Hunt", 40, 40, 32, RAYWHITE);
		DrawText("Raylib setup is working.", 40, 90, 20, LIGHTGRAY);

		/* End drawing frame */
		EndDrawing();
	}

	/* Close the window and clean up resources */
	CloseWindow();

	return (0);
}
#ifndef GAME_H
#define GAME_H

#include "raylib.h"

/**
 * game_run - Runs the main game loop
 *
 * Return: Nothing
 */
void game_run(void);

/* Making texture accessible to raycast.c */
extern Texture2D wall_texture;

extern Texture2D wall_texture;
extern Image wall_image;
extern Color *wall_pixels;

#endif
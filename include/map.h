#ifndef MAP_H
#define MAP_H

#define MAP_WIDTH 10
#define MAP_HEIGHT 10
#define TILE_SIZE 64

/**
 * map_get_tile - Gets the tile value a map coordinate
 * @x: Tile x-cordinate
 * @y: Tile y-cordinate 
 *
 * Return: Tile value, or 1 if outside map boundaries
 */
int map_get_tile(int x, int y);

/**
 * map_is_wall - Checks whether a map coordinate is a wall
 * @x: Tile x-cordinate
 * @y: Tile y-cordinate 
 *
 * Return: 1 if wall, 0 otherwise
 */
int map_is_wall(int x, int y);

#endif
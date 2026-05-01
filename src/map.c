/**
 * @file map.c
 * @brief Fortress map data and map helper functions.
 */
#include "map.h"

/**
 * fortress map - Hardcoded starting map for Thimlich Hunt
 * 
 * 1 = wall
 * 0 = open path
 */
static int fortress_map[MAP_HEIGHT][MAP_WIDTH] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 0, 0, 0, 1, 0, 0, 0, 0, 1},
	{1, 0, 1, 0, 1, 0, 1, 1, 0, 1},
	{1, 0, 1, 0, 0, 0, 0, 1, 0, 1},
	{1, 0, 1, 1, 1, 1, 0, 1, 0, 1},
	{1, 0, 0, 0, 0, 1, 0, 0, 0, 1},
	{1, 1, 1, 1, 0, 1, 1, 1, 0, 1},
	{1, 0, 0, 1, 0, 0, 0, 1, 0, 1},
	{1, 0, 0, 0, 0, 1, 0, 0, 0, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

/**
 * map_get_tile - Gets the tile value at a map coordinate
 * @x: Tile x-coordinate
 * @y: Tile y-coordinate
 * 
 * Return: Tile value, or 1 if outside map boundaries
 */
int map_get_tile(int x, int y){
    if(x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT){
        return (1);
    }
    return(fortress_map[y][x]);
}

/**
 * map_is_wall - Checks whether a map coordinate is a wall
 * @x: Tile x-coordinate
 * @y: Tile y-coordinate
 * 
 * Return: 1 if wall, 0 otherwise
 */
int map_is_wall(int x, int y){
    return(map_get_tile(x,y) == 1);
}
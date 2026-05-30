#ifndef CONFIG_H
#define CONFIG_H

/**
 * Window configuration
 */
#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 720

/**
 * Raycasting configuration
 */
#define FOV 60.0f
#define NUM_RAYS SCREEN_WIDTH
#define WALL_HEIGHT 32000.0f
extern float g_zbuffer[NUM_RAYS];

/**
 * Minimap rendering configuration
 */
#define MAP_RENDER_TILE 16
#define MAP_OFFSET_X 20
#define MAP_OFFSET_Y 520

/**
 * WALL_TEXTURE_REPEAT - Controls how many times the wall texture repeats a single wall tile.
 *
 * Example:
 * 1.0f = one full texture across one wall tile
 * 4.0f = texture repeats 4 times across one wall tile
 *
 * Higher values make the stones appear smaller and more natural.
 */
#define WALL_TEXTURE_REPEAT_X 3.0f

/**
 * WALL_TEXTURE_REPEAT_Y - Controls how many times the wall texture
 * repeats vertically on a wall slice
 */
#define WALL_TEXTURE_REPEAT_Y 1.0f

#endif
/**
 * @file enemy.c
 * @brief Enemy initialization and minimap rendering.
 */
#include "raylib.h"
#include "enemy.h"
#include "map.h"
#include "config.h"


/**
 * enemy_init - Initialize enemy values
 * @enemy: Pointer to enemy structure
 * @tile_x: Enemy starting tile x-coordinate
 * @tile_y: Enemy starting tile y-coordinate
 * 
 * 
 * The enemy is palced at the center of the selected map tile.
 * 
 * Return: Nothing
 */
void enemy_init(Enemy *enemy, int tile_x, int tile_y){
    enemy->x = (tile_x * TILE_SIZE) + (TILE_SIZE / 2.0f);
    enemy->y = (tile_y * TILE_SIZE) + (TILE_SIZE / 2.0f);
    enemy->active = 1;
}


/**
 * enemy_draw_minimap - Draws anemy on minimap
 * @enemy: Pointer to enemy structure
 * 
 * Converts enemy world coordinates into minimap coordinates.
 * 
 * Return: Nothing
 */
void enemy_draw_minimap(Enemy *enemy){
    float scale;
    int screen_x;
    int screen_y;

    /*Do not draw inactive enemies*/
    if(!enemy->active){
        return;
    }

    /* Convert world size to minimap size*/
    scale = (float)MAP_RENDER_TILE / (float)TILE_SIZE;

    /* Convert enemy world positon to minimap position*/
    screen_x = MAP_OFFSET_X + (int)(enemy->x * scale);
    screen_y = MAP_OFFSET_Y + (int)(enemy->y * scale);

    /* Draw enemy as a purple dot on minimap*/
    DrawCircle(screen_x, screen_y, 6, PURPLE);
}

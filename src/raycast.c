/**
 * @file raycast.c
 * @brief Raycasting logic for Thimlich Hunt.
 * 
 */
#include "raylib.h"
#include <math.h>
#include "raycast.h"
#include "map.h"

/**
 * These values must match the map rendering values in game.c
 * 
 * to be moved later to config header
 */
#define MAP_RENDER_TILE 50
#define MAP_OFFSET_X 390
#define MAP_OFFSET_Y 100

/**
 * raycast_draw_single_ray - Draw one ray from the plaher
 * @player: Pointer to player structure
 * 
 * The ray starts from the plaher position and moves forward in the
 * direction the plaher is facing. It advances step by step until it
 * hits a wall or reaches the maximum ray length.
 * 
 * Return: Nothing
 */
void raycast_draw_single_ray(Player *player){
    float ray_x;
    float ray_y;
    float ray_angle;
    float ray_step_x;
    float ray_step_y;
    float scale;
    int map_x;
    int map_y;
    int ray_screen_x;
    int ray_screen_y;
    int player_screen_x;
    int player_screen_y;
    int max_steps;
    int step;

    /* Start the ray at the player's current world position */
    ray_x = player->x;
    ray_y = player->y;

    /* Convert player angle from degrees to radians*/
    ray_angle = player->angle * DEG2RAD;

    /*Calculate how much the ray moves each step */
    ray_step_x = cosf(ray_angle) * 4.0f;
    ray_step_y = sinf(ray_angle) * 4.0f;

    /* Convert world coordinates to rendered map coordinates*/
    scale = (float)MAP_RENDER_TILE / (float)TILE_SIZE;

    /* Convert player world position to screen position*/
    player_screen_x = MAP_OFFSET_X + (int)(player->x * scale);
    player_screen_y = MAP_OFFSET_Y + (int)(player->y * scale);

    /* Limit ray distance so it does not run forever*/
    max_steps = 200;

    for (step = 0; step < max_steps; step++){
        /* Move the ray forward*/
        ray_x += ray_step_x;
        ray_y += ray_step_y;

        /* Convert ray world position to map grid coordinates */
        map_x = (int)(ray_x / TILE_SIZE);
        map_y = (int)(ray_y / TILE_SIZE);

        /*stop ray when it hits a wall*/
        if(map_is_wall(map_x, map_y)){
            break;
        }
    }
    /* Convert final ray hit position to screen coordinates */
    ray_screen_x = MAP_OFFSET_X + (int)(ray_x * scale);
    ray_screen_y = MAP_OFFSET_Y + (int)(ray_y * scale);

    /*Dray the ray from player position to hit point*/
    DrawLine(player_screen_x, player_screen_y, ray_screen_x, ray_screen_y, YELLOW);

    /*Mark the ray hit point*/
    DrawCircle(ray_screen_x, ray_screen_y, 4, ORANGE);
}
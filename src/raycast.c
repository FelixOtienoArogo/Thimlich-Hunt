/**
 * @file raycast.c
 * @brief Raycasting logic for Thimlich Hunt.
 * 
 */
#include "raylib.h"
#include <math.h>
#include "raycast.h"
#include "map.h"
#include "config.h"

/**
 * These values must match the map rendering values in game.c
 * 
 * to be moved later to config header
 */
#define MAP_RENDER_TILE 16
#define MAP_OFFSET_X 20
#define MAP_OFFSET_Y 520
#define FOV 60.0f
#define NUM_RAYS 320
#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 720
#define WALL_HEIGHT 32000.0f

/**
 * raycast_draw_rays - Draws multiple rays (field of view)
 * @player: Pointer to player structure
 * 
 * The rays start from the player position and moves forward in the
 * direction the player is facing. They advances step by step until it
 * hits a wall or reaches the maximum ray
 *  length.
 * Each ray is slightly offset in angle to simulate vision.
 * 
 * Return: Nothing
 */
void raycast_draw_rays(Player *player){
    float ray_x;
    float ray_y;
    float start_angle;
    float ray_angle;
    float angle_step;
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
    int i;

    /* Convert world coordinates to rendered map coordinates*/
    scale = (float)MAP_RENDER_TILE / (float)TILE_SIZE;

    /* Convert player world position to screen position*/
    player_screen_x = MAP_OFFSET_X + (int)(player->x * scale);
    player_screen_y = MAP_OFFSET_Y + (int)(player->y * scale);

    /* Start from left side of FOV*/
    start_angle = player->angle - (FOV / 2.0f);

    /* Angle between each ray*/
    angle_step = FOV / NUM_RAYS;

    for (i = 0; i < NUM_RAYS; i++){
        /* Convert player angle from degrees to radians*/
        ray_angle = (start_angle + i * angle_step) * DEG2RAD;

        /* Start the ray at the player's current world position */
        ray_x = player->x;
        ray_y = player->y;

        /*Calculate how much the ray moves each step */
        ray_step_x = cosf(ray_angle) * 4.0f;
        ray_step_y = sinf(ray_angle) * 4.0f;

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

    }
}


/**
 * raycast_render_3d - Renders pseudo-3D wall slices from raycasts
 * @player: Pointer to player structure
 * 
 * This function casts many rays across the player's field of view.
 * Each ray stops when it hists a wall.
 * 
 * The distacne to the wall controls wall height:
 * -close wall = tall slice
 * -far wall = short slice
 * 
 * Return: Nothing
 */
void raycast_render_3d(Player *player){
    float start_angle;
    float ray_angle;
    float ray_x;
    float ray_y;
    float angle_step;
    float ray_step_x;
    float ray_step_y;
    int map_x;
    int map_y;
    int max_steps;
    int step;
    int ray;
    float distance;
    float corrected_distance;
    float wall_slice_height;
    int slice_x;
    int slice_width;
    int wall_top;
    int wall_bottom;

    /* Start from the left edge of the player's field of view*/
    start_angle = player->angle - (FOV / 2.0f);

    /* Space each ray evenly across the Field of view*/
    angle_step = FOV / NUM_RAYS;

    /*Width of each vertical wall slice on screen*/
    slice_width = SCREEN_WIDTH / NUM_RAYS;

    for(ray = 0; ray < NUM_RAYS; ray++){
        /* Convert player angle from degrees to radians*/
        ray_angle = (start_angle + ray * angle_step) * DEG2RAD;

        /* Start the ray at the player's current world position */
        ray_x = player->x;
        ray_y = player->y;

        /*Calculate how much the ray moves each step */
        ray_step_x = cosf(ray_angle) * 4.0f;
        ray_step_y = sinf(ray_angle) * 4.0f;

        /* Limit ray distance so it does not run forever*/
        max_steps = 300;

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
        /* Calculate raw distance from player to wall hit */
        distance = sqrt(((ray_x - player->x) * (ray_x - player->x)) + ((ray_y - player->y) * (ray_y - player->y)));

        /**
         * Correct fishey distortion
         * Rays at the edge of the FOV travel farther that center rays.
         */
        corrected_distance = distance * cosf(((start_angle + (ray * angle_step)) - player->angle) * DEG2RAD);

        /*Avoid division by zero*/
        if (corrected_distance < 1.0f){
            corrected_distance = 1.0f;
        }

        /**
         * Store corrected wall distance for this ray.
         * Enemy rendering uses this value to determine whether
         * a wall is closer than the enemy
         */
        g_zbuffer[ray] = corrected_distance;

        /* Convert wall distacne into wall height*/
        wall_slice_height = WALL_HEIGHT / corrected_distance;

        /* Keep wall slice from exceeding screen height too much*/
        if(wall_slice_height > SCREEN_HEIGHT){
            wall_slice_height = SCREEN_HEIGHT;
        }

        /* Calculate vertical slice position*/
        wall_top = (SCREEN_HEIGHT / 2) - ((int)wall_slice_height / 2);
        wall_bottom = wall_top + (int)wall_slice_height;

        /* Calculate horizontal screen position*/
        slice_x = ray * slice_width;

        /*Draw sky*/
        DrawRectangle(slice_x, 0, slice_width, wall_top, DARKBLUE);

        /**
         * Shade walls based on distance.
         * Close walls appear brighter, far walls appear darker
         */
        unsigned char shade;

        shade = (unsigned char)(255.0f - (corrected_distance * 0.7f));

        if(shade < 60){
            shade = 60;
        }
        DrawRectangle(slice_x, wall_top, slice_width, wall_bottom - wall_top, (Color){shade, shade, shade, 255});

        /*Draw floor*/
        DrawRectangle(slice_x, wall_bottom, slice_width, SCREEN_HEIGHT - wall_bottom, DARKGREEN);
    }
}
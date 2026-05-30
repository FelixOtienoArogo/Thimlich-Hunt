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
#include "game.h"

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
void raycast_draw_rays(Player *player)
{
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

    for (i = 0; i < NUM_RAYS; i++)
    {
        /* Convert player angle from degrees to radians*/
        ray_angle = (start_angle + i * angle_step) * DEG2RAD;

        /* Start the ray at the player's current world position */
        ray_x = player->x;
        ray_y = player->y;

        /*Calculate how much the ray moves each step */
        ray_step_x = cosf(ray_angle) * 1.0f;
        ray_step_y = sinf(ray_angle) * 1.0f;

        /* Limit ray distance so it does not run forever*/
        max_steps = 1000;

        for (step = 0; step < max_steps; step++)
        {
            /* Move the ray forward*/
            ray_x += ray_step_x;
            ray_y += ray_step_y;

            /* Convert ray world position to map grid coordinates */
            map_x = (int)(ray_x / TILE_SIZE);
            map_y = (int)(ray_y / TILE_SIZE);

            /*stop ray when it hits a wall*/
            if (map_is_wall(map_x, map_y))
            {
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
 * raycast_render_3d - Renders pseudo-3D wall slices using DDA raycasting
 * @player: Pointer to player structure
 *
 * Description:
 * This function casts rays across the player's field of view using
 * the Digital Differential Analyser algorithm. DDA walks from grid
 * boundary to grid boundary instead of moving in small float steps.
 *
 * This gives more stable wall hit detection and smoother wall tops.
 *
 * Return: Nothing
 */
void raycast_render_3d(Player *player)
{
    float start_angle;
    float ray_angle;
    float ray_dir_x;
    float ray_dir_y;
    float delta_dist_x;
    float delta_dist_y;
    float side_dist_x;
    float side_dist_y;
    float perp_wall_dist;
    float wall_slice_height;
    float angle_step;
    int map_x;
    int map_y;
    int step_x;
    int step_y;
    int hit;
    int ray;
    int side;
    int slice_x;
    int slice_width;
    int wall_top;
    int wall_bottom;
    float brightness;
    int texture_y;
    int draw_y;
    float texture_pos;
    float texture_step;
    Color tex_color;
    int texture_x;
    float wall_hit;

    /* Start from the left edge of the player's field of view*/
    start_angle = player->angle - (FOV / 2.0f);

    /* Space each ray evenly across the Field of view*/
    angle_step = FOV / NUM_RAYS;

    /*Width of each vertical wall slice on screen*/
    slice_width = (SCREEN_WIDTH / NUM_RAYS) + 1;

    /* Cast one ray for each configured ray column*/
    for (ray = 0; ray < NUM_RAYS; ray++)
    {
        /* Convert player angle from degrees to radians*/
        ray_angle = (start_angle + ray * angle_step) * DEG2RAD;

        /*Calculate ray direction vector */
        ray_dir_x = cosf(ray_angle);
        ray_dir_y = sinf(ray_angle);

        /*Get current map tile occupied by player */
        map_x = (int)(player->x / TILE_SIZE);
        map_y = (int)(player->y / TILE_SIZE);

        /* Calculate distacne ray travels from one x-side to next x-side, and from one y-side to next y-side*/
        if (ray_dir_x == 0)
        {
            delta_dist_x = 1000000.0f;
        }
        else
        {
            delta_dist_x = fabsf(TILE_SIZE / ray_dir_x);
        }

        if (ray_dir_y == 0)
        {
            delta_dist_y = 1000000.0f;
        }
        else
        {
            delta_dist_y = fabsf(TILE_SIZE / ray_dir_y);
        }

        /**
         * Determine whether ray moves left or right across map tiles.
         * Also calculate initial distance to first vertival grid boundary.
         */
        if (ray_dir_x < 0)
        {
            step_x = -1;
            side_dist_x = (player->x - (map_x * TILE_SIZE)) * delta_dist_x / TILE_SIZE;
        }
        else
        {
            step_x = 1;
            side_dist_x = (((map_x + 1) * TILE_SIZE) - player->x) * delta_dist_x / TILE_SIZE;
        }

        /**
         * Determine whether ray moves up or down across map tiles.
         * Also calculate initial distance to first horizontal grid boundary.
         */
        if (ray_dir_y < 0)
        {
            step_y = -1;
            side_dist_y = (player->y - (map_y * TILE_SIZE)) * delta_dist_y / TILE_SIZE;
        }
        else
        {
            step_y = 1;
            side_dist_y = (((map_y + 1) * TILE_SIZE) - player->y) * delta_dist_y / TILE_SIZE;
        }

        /* Reset hit state before DDA traversal*/
        hit = 0;
        side = 0;

        /**
         * DDA loop:
         * Move to the next closest grid boundary until a wall tile is hit.
         */
        while (!hit)
        {
            /* Move to next vertical or horizontal grid boundary*/
            if (side_dist_x < side_dist_y)
            {
                side_dist_x += delta_dist_x;
                map_x += step_x;
                side = 0;
            }
            else
            {
                side_dist_y += delta_dist_y;
                map_y += step_y;
                side = 1;
            }

            /* Stop when ray reaches a wall tile*/
            if (map_is_wall(map_x, map_y))
            {
                hit = 1;
            }
        }

        /**
         * Calculate perpendicular wall distance.
         * This avoids fisheye distortion
         */
        if (side == 0)
        {
            perp_wall_dist = side_dist_x - delta_dist_x;
        }
        else
        {
            perp_wall_dist = side_dist_y - delta_dist_y;
        }

        /*Avoid division by zero or extremely large wall slices*/
        if (perp_wall_dist < 1.0f)
        {
            perp_wall_dist = 1.0f;
        }

        /**
         * Store wall distance for enemy/object depth checking.
         * Enemy rendering uses this to avoid drawing through walls.
         */
        g_zbuffer[ray] = perp_wall_dist;

        /* Convert wall distance into vertical wall height */
        wall_slice_height = WALL_HEIGHT / perp_wall_dist;

        /* Keep wall slice from exceeding screen height too much*/
        if (wall_slice_height > SCREEN_HEIGHT)
        {
            wall_slice_height = SCREEN_HEIGHT;
        }

        /* Calculate vertical slice position*/
        wall_top = (SCREEN_HEIGHT / 2) - ((int)wall_slice_height / 2);
        wall_bottom = wall_top + (int)wall_slice_height;

        /* Calculate horizontal screen position*/
        slice_x = ray * slice_width;

        /**
         * Draw darker sky/ceiling above the wall.
         * This gives the scene a moodier fortress atmosphere.
         */
        DrawRectangle(
            slice_x,
            0,
            slice_width,
            wall_top,
            (Color){20, 45, 75, 255});

        /**
         * Calculate exact wall hit position.
         * If we hit an X-side wall, use Y position for texture X.
         * If we hit a Y-side wall, use X position for texture X.
         */
        if (side == 0)
        {
            wall_hit = player->y + (perp_wall_dist * ray_dir_y);
        }
        else
        {
            wall_hit = player->x + (perp_wall_dist * ray_dir_x);
        }

        /* Keep only the position inside the current tile */
        wall_hit -= floorf(wall_hit / TILE_SIZE) * TILE_SIZE;

        /**
         * Convert wall hit position into texture X coordinate.
         * This maps the horizontal hit position to the texture width.
         */
        texture_x = (int)(fmodf(
                              (wall_hit / TILE_SIZE) * WALL_TEXTURE_REPEAT_X, 1.0f) *
                          wall_image.width);

        /* Clamp texture X to avoid image range */
        if (texture_x < 0)
        {
            texture_x = 0;
        }
        if (texture_x >= wall_image.width)
        {
            texture_x = wall_image.width - 1;
        }

        /* Protection from division by zero */
        if ((wall_bottom - wall_top) <= 0)
        {
            continue;
        }

        /**
         * texture_step tells us how much to move in teh texture
         * for each screen pixel drwan vertically.
         */
        texture_step = (float)wall_image.height / (float)(wall_bottom - wall_top);

        /**
         * Prevent excessive vertical texture stretching when
         * extremely close to walls.
         */
        if (texture_step < 0.25f)
        {
            texture_step = 0.25f;
        }

        /**
         * Start texture position.
         * If wall_top is abobe the screen, skip the hidden texture area.
         */
        texture_pos = 0.0f;
        if (wall_top < 0)
        {
            texture_pos = -wall_top * texture_step;
        }

        /* Clamp drawing area to screen */
        if (wall_top < 0)
        {
            wall_top = 0;
        }
        if (wall_bottom > SCREEN_HEIGHT)
        {
            wall_bottom = SCREEN_HEIGHT;
        }

        /**
         * Draw wall column pixel by pixel.
         * Each screen Y samples the correct texture Y.
         */
        for (draw_y = wall_top; draw_y < wall_bottom; draw_y++)
        {
            texture_y = (int)texture_pos % wall_image.height;

            tex_color = wall_pixels[(texture_y * wall_image.width) + texture_x];

            /**
             * Apply simple distance/side shading.
             * This keeps depth while preserving texture detail.
             */
            brightness = 255.0f - (perp_wall_dist * 0.25f);

            if (brightness < 80.0f)
            {
                brightness = 80.0f;
            }
            if (brightness > 255.0f)
            {
                brightness = 255.0f;
            }
            if (side == 1)
            {
                brightness *= 0.75f;
            }

            tex_color.r = (unsigned char)((tex_color.r * brightness) / 255.0f);
            tex_color.g = (unsigned char)((tex_color.g * brightness) / 255.0f);
            tex_color.b = (unsigned char)((tex_color.b * brightness) / 255.0f);

            DrawLine(slice_x, draw_y, slice_x + slice_width, draw_y, tex_color);

            texture_pos += texture_step;
        }

        /**
         * Draw darker ground/floor below the wall.
         * Brown/earth tone fits the Thimlich Ohinga setting better
         * than bright green.
         */
        DrawRectangle(
            slice_x,
            wall_bottom,
            slice_width,
            SCREEN_HEIGHT - wall_bottom,
            (Color){55, 42, 28, 255});
    }
}
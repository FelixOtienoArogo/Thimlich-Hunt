/**
 * @file enemy.c
 * @brief Enemy initialization and minimap rendering.
 */
#include "raylib.h"
#include "enemy.h"
#include "map.h"
#include "config.h"
#include <math.h>
#include "player.h"


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


/**
 * enemy_detect_player - Checks if enemy detects player
 * @enemy: Pointer to enemy
 * @player: Pointer to plaher
 * 
 * Uses distance between player and enemy.
 * 
 * Return: 1 if detected, 0 otherwise
 */
int enemy_detect_player(Enemy *enemy, Player *player){
    float dx;
    float dy;
    float distance;

    if(!enemy->active){
        return(0);
    }

    /* Distacne formula*/
    dx = player->x - enemy->x;
    dy = player->y - enemy->y;

    distance = sqrtf((dx * dx) + (dy * dy));

    /* Detection radius (adjust later)*/
    if (distance < 120.0f){
        return(1);
    }
    return(0);
}

/**
 * enemy_update - Update enemy behavior
 * @enemy: Pointer to enemy
 * #player: Pointer to player
 * 
 * 
 * This is the first enemy AI behavior:
 * -IF the enmy is inactive, do nothing
 * -If the enemy is within detection range, move toward the player
 * _Movement is normalized so diagonal movement is not faster.
 * 
 * Return: Nothing
 */
void enemy_update(Enemy *enemy, Player *player){
    float dx;
    float dy;
    float distance;
    float enemy_speed;

    /*Ignore enemies that are no longer active*/
    if(!enemy->active){
        return;
    }
    
    /*Only chase if the player is detected*/
    if(!enemy_detect_player(enemy, player)){
        return;
    }

    /* Calculate direction from enemy to player*/
    dx = player->x - enemy->x;
    dy = player->y - enemy->y;

    /* Calculate distance between enemy and player*/
    distance = sqrtf((dx * dx) + (dy * dy));

    /* Avoid division by zero if enemy is exactly on player*/
    if (distance <= 1.0f){
        return;
    }

    /* Enemy movement speed per frame*/
    enemy_speed = 1.0f;

    /**
     * Normalize direction:
     * dx/ distance and dy/distance produce a unit direction vector
     * Multiplying by speed gives controlled mvement towards the player
     */
    enemy->x += (dx / distance) * enemy_speed;
    enemy->y += (dy / distance) * enemy_speed;
}
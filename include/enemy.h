#ifndef ENEMY_H
#define ENEMY_h

#include "player.h"

/**
 * struct Enemy - stores enemy state
 * @x: Enemy x-position in world coordinates
 * @y: Enemy y-position in world coordinates
 * @active: Whether enemy is currently alive/active
 * 
 */
typedef struct Enemy{
    float x;
    float y;
    int active;
} Enemy;

/**
 * enemy_init - Intializes enemy values
 * @enemy: Pointer to enemy structure
 * @tile_x: Enemy starting tile x-coordinate
 * @tile_y: Enemy starting tile y-coordinate
 * 
 * Return: Nothing
 */
void enemy_init(Enemy *enemy, int tile_x, int tile_y);

/**
 * enemy_draw_mining - Draws enemy on minimap
 * Wenemy: Pointer to enemy structure
 * 
 * Return: Nothing
 */
void enemy_draw_minimap(Enemy *enemy);

/**
 * enemy_detect_player - Checks if enemy detects player
 * @enemy: Pointer to enemy
 * @player: Pointer to player
 * 
 * Return: 1 if player detected, 0 otherwise
 */
int enemy_detect_player(Enemy *enemy, Player *player);

/**
 * enemy_update - Updates enemy behavior
 * @enemy: Pointer to enemy
 * @player: Pointer to player
 * 
 * If the player is close enough, the enemy moves toward the player
 * 
 * Return: Nothing
 */
void enemy_update(Enemy *enemy, Player *player);

/**
 * enemy_check_plaher_contact - Checks enemy contact with player
 * @enemy: Pointer to enemy
 * @player: Pointer to player
 * 
 * Return: 1 if anamy touches player, 0 otherwise
 */
int enemy_check_player_contact(Enemy *enemy, Player *player);

#endif

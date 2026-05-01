#ifndef ENEMY_H
#define ENEMY_h

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

#endif

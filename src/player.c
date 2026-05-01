/**
 * @file player.c
 * @brief Player movement, rotation, and collision checks.
 */

 #include "raylib.h"
 #include "player.h"
 #include "map.h"
 #include <math.h> /* for cosf, sinf */
 #include <stdio.h>

 /**
  * player_init - Initializes player values
  * @player: Pointer to player structure
  * 
  * We place the player slighltly inside tile so they don't start in a wall
  * TILE_SIZE * 1.5f > center-ish of the tile.
  * 
  * Return: Nothing
  */
 void player_init(Player *player){
    /* Start player in the center of open tile (1, 1)*/
    player->x = (1.0f * TILE_SIZE) + (TILE_SIZE / 2.0f);
    player->y = (1.0f * TILE_SIZE) + (TILE_SIZE / 2.0f);

    /* Angle in degrees. 0 means facing to the right */
    player->angle = 0.0f;

    /* Pixels moved per frame when pressing forward/backward */
    player->speed = 2.5f;

    /* Degree turned per frame when pressing left/right*/
    player->rotation_speed = 2.5f;
 }


 /**
  * player_can_move - Checks if player can move to target position
  * @x: Target x-position
  * @y: Target y-position
  * 
  * Convert world position (pixels) to map grid coordinates:
  *  map_x = x / TILE_SIZE
  *  map_y = y / TILE_SIZE
  * 
  * Then query the map:
  *  1 > wall
  *  0 > empty
  * 
  * Return: 1 if movement is allowed, 0 otherwise
  */
 static int player_can_move(float x, float y){
    int map_x;
    int map_y;

    /* Convert pixel position to tile indices */
    map_x = (int)(x / TILE_SIZE);
    map_y = (int)(y / TILE_SIZE);

    /* If target tile is a wall, movement is not allowed */
    if(map_is_wall(map_x, map_y)){
        return(0);
    }
    return (1);
 }

 /**
  * player_update - Updates player movement and rotation
  * @player: Pointer to player structure
  * 
  * 
  * Steps:
  * 1. Read rotation input and update angle
  * 2. Compute forward/backward movement vector based on angle
  * 3. Check collision for the target position
  * 4. Apply movement if allowed
  */
 void player_update(Player *player){
    /*Accumulated movement on x and Y axis for this frame*/
    float move_x;
    float move_y;
    float angle_rad; /* Angle converted to radians for trig fuctions*/

    /*Convert degrees to radians bcoz cosf/sif expect radians*/
    angle_rad = player->angle * DEG2RAD;

    /* Reset movement each frame */
    move_x = 0.0f;
    move_y = 0.0f;

    /* Turn left*/
    if(IsKeyDown(KEY_LEFT)){
        player->angle -= player->rotation_speed;
        printf("Left key press\n %.02f", player->angle);
    }

    /* Turn right */
    if(IsKeyDown(KEY_RIGHT)){
        player->angle += player->rotation_speed;
    }

    /**
     * Forward movement:
     * cos(angle) > X direction
     * sin(angle) > Y direction
     * 
     * This gives a vector pointing where the player is facing.
     */
    if(IsKeyDown(KEY_UP)){
        move_x += cosf(angle_rad) * player->speed;
        move_y += sinf(angle_rad) * player->speed;
    }

    /**
     * Backward movement:
     * cos(angle) > X direction
     * sin(angle) > Y direction
     * 
     * This gives a vector pointing where the player is facing.
     */
    if(IsKeyDown(KEY_DOWN)){
        move_x -= cosf(angle_rad) * player->speed;
        move_y -= sinf(angle_rad) * player->speed;
    }


    /**
     * -----------------COLLISION CHECK---------------
     * Before applying movement, we test the future position
     * If that position is not inside a wall > allow movement
     */
    if(player_can_move(player->x + move_x, player->y + move_y)){
        player->x += move_x;
        player->y += move_y;
    }
 }
 
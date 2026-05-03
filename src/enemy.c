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
    if(!enemy){
        return;
    }

    /* Convert tile position to center world coordinates */
    enemy->x = (tile_x * TILE_SIZE) + (TILE_SIZE / 2.0f);
    enemy->y = (tile_y * TILE_SIZE) + (TILE_SIZE / 2.0f);

    /* Give enemy starting health*/
    enemy->health = 3;

    /* Mark enemy as active*/
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
 * enemy_can_move - Checks whether enemy can move ot target position
 * @x: Targert x-positon in world coordinates
 * @y: Target y-positon in world coordinates
 * 
 * The target world posiotn is covertedin to map tile coordintesl
 * If the target tile is a wall, movement is blocked
 * 
 * Return: 1 if movement is allowed, 0 otherwise
 */
static int enemy_can_move(float x, float y){
    int map_y;
    int map_x;

    /* Convert world coordinates into map tile coordinates*/
    map_x = (int)(x / TILE_SIZE);
    map_y = (int)(y / TILE_SIZE);

    /* Block movement into walls*/
    if (map_is_wall(map_x, map_y)){
        return (0);
    }
    return (1);
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
     * Only move if the tarfet position odes not collide with a wall.
     */
    if(enemy_can_move(enemy->x + ((dx / distance) * enemy_speed), enemy->y + ((dy / distance) * enemy_speed))){
        enemy->x += (dx / distance) * enemy_speed;
        enemy->y += (dy / distance) * enemy_speed;  
    }
}

/**
 * enemy_check_player_contact - Checks enemy contact with player
 * @enemy: Pointer to enemy
 * @plaher: Pointer to plaher
 * 
 * 
 * Uses disstacne between enemy and plaher to detect contact.
 * 
 * Return: q if enemy touches plaher, 0 otherwise
 */
int enemy_check_player_contact(Enemy *enemy, Player *player){
    float dx;
    float dy;
    float distance;

    /* Ignore inactive enemy*/
    if(!enemy->active){
        return (0);
    }

    /* Calculate the distance between enemy and player*/
    dx = player->x - enemy->x;
    dy = player->y - enemy->y;

    distance = sqrtf((dx * dx) + (dy * dy));

    /*If enemy is close return true*/
    if(distance < 20.0f){
        return(1);
    }
    return(0);
}

/**
 * enemy_render_3d - Renders enemy in pseudo-3D view
 * @enemy: Pointer to enemy structure
 * @player: Pointer to plaher structure
 * 
 * Return: Nothing
 */
void enemy_render_3d(Enemy *enemy, Player *player, float *zbuffer){
    float dx;
    float dy; 
    float distance;
    float forward_x;
    float forward_y;
    float dot;
    float angle_to_enemy;
    float angle_diff;
    float fov;
    float screen_x;
    float enemy_height;
    float enemy_width;
    float top;
    float left;
    int ray_index;
    float player_angle_rad;
    int head_radius;
    int head_x;
    int head_y;
    int body_x;
    int body_y;
    int body_width;
    int body_height;
    unsigned char enemy_shade;
    Color enemy_head_color;
    Color enemy_body_color;

    /* Validate pointers and Skip render ing if enemy is inactive*/
    if (!enemy || !player || !zbuffer || !enemy->active){
        return;
    }

    /*Convert player angle from degrees to radians*/
    player_angle_rad = player->angle * DEG2RAD;

    /* Calculate distacne vector between player and enemy*/
    dx = enemy->x - player->x;
    dy = enemy->y - player->y;

    distance = sqrtf((dx * dx) + (dy * dy));

    /*Avoid rendering if enemy is too close */
    if(distance < 1.0f){
        return;
    }

    /*Calculates plaher's forward direction vector*/
    forward_x = cosf(player_angle_rad);
    forward_y = sinf(player_angle_rad);

    /* Use dot product to check if enemy is in front of player. Negative is behing*/
    dot = (dx * forward_x) + (dy * forward_y);

    /* Do not render if enemy is behind player*/
    if(dot <= 0){
        return;
    }

    /* Calculate angle from plaher to enemy*/
    angle_to_enemy = atan2f(dy, dx);

    /* Calculate difference between plaher's view angle and enemy angle*/
    angle_diff = angle_to_enemy - player_angle_rad;

    /* Normalize anlge to range [-PI, PI] for correct comparison*/
    while (angle_diff > PI){
        angle_diff -= (2 * PI);
    }
    while ((angle_diff < - PI))
    {
        angle_diff += (2 * PI);
    }
    
    /* Define player's fov(60 degrees)*/
    fov = 60.0f * DEG2RAD;

    /*Skip rendering if enemy is outside the field of view*/
    if (fabsf(angle_diff) > (fov / 2)){
        return;
    }

    /* Convert enemy angle into horizotnal screen position */
    screen_x = (SCREEN_WIDTH / 2) + ((angle_diff / (fov / 2)) * (SCREEN_WIDTH / 2));

    /* Skip enemy if projected completely outside the screen*/
    if (screen_x < 0 || screen_x >= SCREEN_WIDTH){
        return;
    }

    /* Convert screen position into matcing ray index*/
    ray_index = (int)((screen_x / SCREEN_WIDTH) * NUM_RAYS);

    /*Skip enemy if projected outisde screen*/
    if(ray_index < 0 || ray_index >= NUM_RAYS){
        return;
    }

    /**
     * Depth check:
     * If wall distacne at this screen column is smaller than enemy distacne,
     * the wall is closer, so the enemy must be hidden
     */
    if(distance > zbuffer[ray_index]){
        return;
    }

    /**
     * Calculate enemy brightness based on distance.
     * Close enemies appear brighter, far enemies appear darker.
     */
    enemy_shade = (unsigned char)(255.0f - (distance * 0.7f));

    /* Keep enemy visible even when far away*/
    if (enemy_shade < 80){
        enemy_shade = 80;
    }

    /* Build shaded enemy colors*/
    enemy_head_color = (Color){
        enemy_shade, enemy_shade / 2, 0, 255
    };
    enemy_body_color = (Color){enemy_shade, 0, 0, 255};

    /* Scale enemy size based on distance*/
    enemy_height = (TILE_SIZE * SCREEN_HEIGHT) / distance;
    enemy_width = enemy_height * 0.5f;

    /*Prevent enemy rectangle from becoming too large*/
    if(enemy_height > SCREEN_HEIGHT){
        enemy_height = SCREEN_HEIGHT;
    }
    if(enemy_width > (SCREEN_WIDTH / 2)){
        enemy_width = SCREEN_WIDTH / 2;
    }

    /* Calculate top-left positon for drawing enemy*/
    top = (SCREEN_HEIGHT / 2) - (enemy_height / 2);
    left = screen_x - (enemy_width / 2);

    /*Skip enemy if the full rectangle is outside the screen*/
    if ((left + enemy_width) < 0 || left >= SCREEN_WIDTH){
        return;
    }

    /* Size the head relative to total enemy height */
    head_radius = (int)(enemy_width * 0.35f);

    /* Position the head at the upper section of the enemy*/
    head_x = (int)screen_x;
    head_y = (int)(top + (enemy_height * 0.22f));

    /* Position the body below the head*/
    body_width = (int)(enemy_width * 0.70f);
    body_height = (int)(enemy_height * 0.55f);
    body_x = (int)(screen_x - (body_width / 2));
    body_y = (int)(top + (enemy_height * 0.38f));

    /* Draw enemy head*/
    DrawCircle(head_x, head_y, head_radius, enemy_head_color);

    /* Draw enemy body */
    DrawRectangle(body_x, body_y, body_width, body_height, enemy_body_color);
}
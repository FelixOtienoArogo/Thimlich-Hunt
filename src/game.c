/**
 * @file game.c
 * @brief Main game loop and game-state orchestration.
 */

 #include "raylib.h"
 #include "game.h"
 #include "player.h"
 #include "map.h"
 #include "raycast.h"
 #include <math.h>
 #include "config.h"
 #include "enemy.h"
 #include "stdio.h"

 /* Global (module-local) player instance */
static Player player;
/**
 * MAX_ENEMIES - Maximum number of enemies active in the game
 */
#define MAX_ENEMIES 3

/*Global module-local enemy list */
static Enemy enemies[MAX_ENEMIES];

/* Tracks whether the minimap is visible */
static int show_minimap = 1;

/* Controls the red screen flash shown when player takes damage */
static int damage_flash_timer = 0;

/* Prevents player from taking damage every frame*/
static int damage_cooldown_timer = 0;

/* Prevent player from attacking every frame */
static int attack_cooldown_timer = 0;

/* Controls short weapon swing visual feedback*/
static int attack_flash_timer = 0;

/* Controls short attack result text feedback*/
static int attack_text_timer = 0;

/*Stores last attack result: 1 = hit, 2 = miss*/
static int attack_result = 0;

/**
 * g_zbuffer - Stores wall distacne for each screen column
 * 
 * Description:
 * Each index represents a vertical screen column.
 * The value stored is the distance fromthe player to the wall
 * rendered at that column.
 */
float g_zbuffer[NUM_RAYS];

/**
 * enum GameState - Represents the current game state
 * @GAME_PLAYING: Normal gameplay is active
 * @GAME_OVER: Player has lost all health
 * @GAME_VICTORY: All enemies have been defeated
 */
typedef enum GameState{
    GAME_PLAYING,
    GAME_OVER,
    GAME_VICTORY
} GameState;

/* Tracks the current state of the game*/
static GameState game_state = GAME_PLAYING;

/**
 * struct ControlPoint - Represetns the castle reclaim objective
 * @x: Control point x-position in world coordinates
 * @y: Control point y-position in world coordinates
 * @active: whether the control point can currently be used
 */
typedef struct ControlPoint{
    float x;
    float y;
    int active;
} ControlPoint;

/*Castle objective used to reclaim the fortress*/
static ControlPoint control_point;


 /**
  * process_input - Handles player and system input
  * 
  * M enables the minimap.
  * N disables the minimap
  * 
  * Return: Nothing
  * 
  */
 static void process_input(void){
    /*Enables minimap*/
    if(IsKeyPressed(KEY_M)){
        show_minimap = 1;
    }
    /*Disable minimap*/
    if(IsKeyPressed(KEY_N)){
        show_minimap = 0;
    }
 }

 /**
  * control_point_init - Initializes the fortress control point
  * @tile_x: Control point tile x-coordinate
  * @tile_y: Contorl point tile y-coordinate
  * 
  * Description:
  * The control point is placed at the center of a map tile.
  * The player must reach this point after defeating all enemies
  * to reclaim the castle.
  * 
  * Return: Nothing
  */
 static void control_point_init(int tile_x, int tile_y){
    control_point.x = (tile_x * TILE_SIZE) + (TILE_SIZE / 2);
    control_point.y = (tile_y * TILE_SIZE) + (TILE_SIZE / 2);
    control_point.active = 0;
 }

 /**
  * player_reached_control_point - Checks if player reached objective
  * 
  * Description:
  * The control point only matters after all enemis are defeated.
  * If the player is close enough, the castler is reclaimed.
  * 
  * Return: 1 if player reached control point, otherwise 0
  */
 static int player_reached_control_point(void){
    float dx;
    float dy;
    float distance;

    /* Contorl point must be active before it can trigger victory */
    if(!control_point.active){
        return(0);
    }

    /* Calculate distacne between player and control point*/
    dx = control_point.x - player.x;
    dy = control_point.y - player.y;
    distance = sqrtf((dx * dx) + (dy * dy));


    /*Player is close enough to reclim the fortress*/
    if (distance < TILE_SIZE * 0.6f){
        return(1);
    }
    return(0);
 }

/**
 * reset_game - Resets player and enemy state
 * 
 * Description:
 * This function restores the player and enemy to their starting
 * positons and clears damage feedback timers
 * 
 * Return: Nothing
 */
static void reset_game(void){
    player_init(&player);

    /* Place Fortress Heart objective inside the castle*/
    control_point_init(4,5);

    /**
     * Place enemies on different walkable map tiles.
     * These tile positons can be adjusted as the map evolves.
     */
    enemy_init(&enemies[0], 8, 8);
    enemy_init(&enemies[1], 7, 5);
    enemy_init(&enemies[2], 8, 7);

    damage_flash_timer = 0;
    damage_cooldown_timer = 0;

    /* Return game to normal playing state*/
    game_state = GAME_PLAYING;
}

/**
 * player_attack - Checks if player hits an enemy
 * 
 * Description:
 * This function performs a simple melee attack when SPACE is pressed.
 * An enemy is hit if:
 * -It is active
 * -It is close enough to the player
 * -It is infront of the player
 * 
 * Return: Nothing
 */
static void player_attack(void){
    int i;
    float dx;
    float dy;
    float distance;
    float player_angle_rad;
    float forward_x;
    float forward_y;
    float dot;

    /* Only attack when SPACE is pressed and cooldown is ready*/
    if(!IsKeyPressed(KEY_SPACE) || attack_cooldown_timer > 0){
        return;
    }

    /* Convert player viewing angle from degrees to radians*/
    player_angle_rad = player.angle * DEG2RAD;

    /* Calculate player's forward direction*/
    forward_x = cosf(player_angle_rad);
    forward_y = sinf(player_angle_rad);

    /* Check all enemies for a possible hit*/
    for (i = 0; i < MAX_ENEMIES; i++){
        /* Skip enemies that are already inactive*/
        if(!enemies[i].active){
            continue;
        }

        /* Calculate enemy position relative to player*/
        dx = enemies[i].x - player.x;
        dy = enemies[i].y - player.y;

        /*Calculate distance from player to enemy*/
        distance = sqrtf((dx * dx) + (dy * dy));

        /*Skip enemy if it is outside melee attack range*/
        if(distance > TILE_SIZE * 1.2f){
            continue;
        }

        /**
         * Dot product checks whether enemy is in front of player.
         * Higher value mens enemy is more directly ahead.
         */
        dot = (dx * forward_x) + (dy * forward_y);

        /* Skip enemy if not clearly in front of player */
        if(dot < distance * 0.6f){
            continue;
        }

        /* Reduce enemy health after successful hit*/
        enemies[i].health -= 1;

        /*Deactivate enemy only when health reaches zero*/
        if(enemies[i].health <= 0){
            enemies[i].active = 0;
        }

        /*Start cooldown after attack and visual feedback*/
        attack_cooldown_timer = 20;
        attack_flash_timer = 6;
        attack_text_timer = 20;
        attack_result = 1;

        return;
    }

    /*Start cooldown and miss foeedback even if attack misses */
    attack_cooldown_timer = 20;
    attack_flash_timer = 6;
    attack_text_timer = 20;
    attack_result = 2;
}

/**
 * all_enemies_defeated - Checks if all enemies are inactive
 * 
 * Description:
 * This function loops through the enemy list and checks whether
 * any enemy is still active. If no active enemies remain, the
 * player has completed the combat objective.
 * 
 * Return:  1 if all enemies are defeated, otherwise 0
 */
static int all_enemies_defeated(void){
    int i;

    /* Check every enemy in the current level */
    for(i = 0; i < MAX_ENEMIES; i++){
        /* If one enemy is still active, victory is not achieved*/
        if(enemies[i].active){
            return (0);
        }
    }

    /* No active enemies remain*/
    return (1);
}

 /**
  * update game - Updates the game state
  * 
  * Return: Nothing
  */
 static void update_game(void){
    int i;
    int player_hit;

    /**
     * If game is over or won, stop normal gameplay updates..
     * Pressing R restarts the game state.
     */
    if(game_state == GAME_OVER || game_state == GAME_VICTORY){
        if(IsKeyPressed(KEY_R)){
            reset_game();
        }
        return;
    }

    /* Update player (movement + colision)*/
    player_update(&player);

    /* Handle player melee attack */
    player_attack();

    /* Tarack whether any enemy has touched the player this frame */
    player_hit = 0;

    /* Update all active enemies*/
    for (i = 0; i < MAX_ENEMIES; i++){
        /*Update enemy behavior*/
        enemy_update(&enemies[i], &player);

        if (enemy_check_player_contact(&enemies[i], &player)){
            player_hit = 1;
        }
    }

    /**
     * Apply damage only once per cooldown window, even if multiple
     * enemies touch the plaher at the same time.
     */
    if(player_hit && player.health > 0 && damage_cooldown_timer == 0){
        player.health -= 1;
        damage_flash_timer = 10;
        damage_cooldown_timer = 30;
    }

    /* Reduce flash timer every frame until the flash disappears*/
    if(damage_flash_timer > 0){
        damage_flash_timer--;
    }

    /* Reduce damage cooldown every frame. Prevents health form dropping too quickly*/
    if(damage_cooldown_timer > 0){
        damage_cooldown_timer--;
    }

    /* Reduce attack cooldown every frame. This prevents attack spamming.*/
    if(attack_cooldown_timer > 0){
        attack_cooldown_timer--;
    }

    /* Reduce weapon swing flash timer*/
    if(attack_flash_timer > 0){
        attack_flash_timer--;
    }

    /*Reduce attack result text timer*/
    if(attack_text_timer > 0){
        attack_text_timer--;
    }

    /* Switch to game-over state when player health reaches zero*/
    if(player.health <= 0){
        game_state = GAME_OVER;
        return;
    }

    /**
     * Activate the Fortress Hearth after all enemies are defeated.
     * The player must still reach it to reclaim the castle.
     */
    if(all_enemies_defeated()){
        control_point.active = 1;
    }

    /* Reclaim castle only after reaching the active control point*/
    if(player_reached_control_point()){
        game_state = GAME_VICTORY;
    }
 }

/**
 * render_map - Draws the fortress map ina top-down view
 * 
 * This function iterates through the 2D map grid and renders:
 * -Walls as filled rectangles
 * -Empty spaces as outlined rectangles
 * 
 * Each tile is scaled using TILE_SIZE to convert grid coordinates
 * into screen pixel coordinates
 * 
 * Return: Nothing
 */
static void render_map(void){
    int x;
    int y;
    int tile;
    int screen_x;
    int screen_y;

    /* Loop through each row of the map*/
    for (y = 0; y < MAP_HEIGHT; y++){
        /* Loop through each column of the map*/
        for (x = 0; x < MAP_WIDTH; x++){
            /* Get the tile value at (x, y)*/
            tile = map_get_tile(x,y);

            /*Convert map grid position to screen position*/
            screen_x = MAP_OFFSET_X + (x * MAP_RENDER_TILE);
            screen_y = MAP_OFFSET_Y + (y * MAP_RENDER_TILE);

            if(tile == 1){
            /* Draw wall tile*/
            DrawRectangle(
                screen_x,
                screen_y,
                MAP_RENDER_TILE,
                MAP_RENDER_TILE,
                LIGHTGRAY
            );
            }
            else{
                /*Walkable floor tile*/
                DrawRectangle(
                    screen_x,
                    screen_y,
                    MAP_RENDER_TILE,
                    MAP_RENDER_TILE,
                    BLACK
                );
                /* Grid outline for visibility*/
                DrawRectangleLines(screen_x, screen_y,
					MAP_RENDER_TILE, MAP_RENDER_TILE, DARKGRAY);
            }
        }
    }
}

/**
 * sort_enemies_by_distance - sort enemies from far to near
 * @player: Pointer to plaher
 * 
 * Description:
 * This uses a simple bubble sort to order enemies so that
 * farther enemies are rendered first, and closer ones last.
 * 
 * Return: Nothing
 */
static void sort_enemies_by_distance(Player *player){
    int i;
    int j;
    float dist_a;
    float dist_b;
    Enemy tmp;

    /* Loop through all enemies except the last one*/
    for (i = 0; i < MAX_ENEMIES; i++){
        /* Compare adjacent enemies*/
        for(j = 0; j < MAX_ENEMIES - i - 1; j++){
            /**
             * Calculate distance from player to current enemy
             * Using Euclidean distance formula
             */
            dist_a = sqrtf(
                (enemies[j].x - player->x) *
                (enemies[j].x - player->x) +
                (enemies[j].y - player->y) *
                (enemies[j].y - player->y)
            );

            /**
             * Calculate distance from player to next enemy
             */
            dist_b = sqrtf(
                (enemies[j + 1].x - player->x) *
                (enemies[j + 1].x - player->x) +
                (enemies[j + 1].y - player->y) *
                (enemies[j + 1].y - player->y)
            );

            /**
             * Swap enemies if current is closer than next
             * This ensures farther enemies come first
             */
            if(dist_a < dist_b){
                tmp = enemies[j];
                enemies[j] = enemies[j + 1];
                enemies[j + 1] = tmp;
            }
        }
    }
}

/**
 * render_control_point_3d - Renders Fortress Heart in pseudo-3D view
 * 
 * Description:
 * This function projects the Fortress Heart from 2D world coordinates
 * into the player's 3D view. It uses the same billboard-style rendering
 * approach as enemies.
 * 
 * 
 * The control point only renders when active, and it uses the z-buffer
 * so it does not appear through walls.
 * 
 * Return: Nothing
 */
static void render_control_point_3d(void){
    float dx;
    float dy;
    float distance;
    float player_angle_rad;
    float angle_to_point;
    float angle_diff;
    float screen_x;
    float point_height;
    float point_width;
    float top;
    float left;
    int ray_index;

    /* Do not render Fortress Heart until it is active */
    if(!control_point.active){
        return;
    }

    /* Calculate control_point position relative to player */
    dx = control_point.x - player.x;
    dy = control_point.y - player.y;

    /* Calculate distance between player and control point*/
    distance = sqrtf((dx * dx) + (dy * dy));

    /* Avoid rendering too close to prevetn extreme scaling*/
    if(distance < 1.0f){
        return;
    }

    /* Convert player angle from degrees to radians*/
    player_angle_rad = player.angle * DEG2RAD;

    /* Calculate angle from player to control point*/
    angle_to_point = atan2f(dy, dx);

    /* Calculate difference between view direction and control point*/
    angle_diff = angle_to_point - player_angle_rad;

    /* Normalize angle difference to range [-PI, PI]*/
    while(angle_diff > PI){
        angle_diff -= (2 * PI);
    }
    while(angle_diff < -PI){
        angle_diff += (2 * PI);
    }

    /* Skip rendering if control point is outside field of view */
    if(fabsf(angle_diff) > ((FOV * DEG2RAD) / 2)){
        return;
    }

    /* Convert angle difference into screen X position*/
    screen_x = (SCREEN_WIDTH / 2) + ((angle_diff / ((FOV * DEG2RAD) / 2)) * (SCREEN_WIDTH / 2));

    /* Skip if projected outside screen */
    if(screen_x < 0 || screen_x >= SCREEN_WIDTH){
        return;
    }

    /* Convert screen x position into matching z-buffer ray index*/
    ray_index = (int)((screen_x / SCREEN_WIDTH) * NUM_RAYS);

    /* Validate ray index before reading z-buffer*/
    if(ray_index < 0 || ray_index >= NUM_RAYS){
        return;
    }

    /* Hide Fortress Heart if a wall is closer */
    if(distance > g_zbuffer[ray_index]){
        return;
    }

    /* Scale marker size based on distance */
    point_height = (TILE_SIZE * SCREEN_HEIGHT) / distance;
    point_width = point_height * 0.45f;

    /* Clamp marker size so it does not become too large*/
    if(point_height > SCREEN_HEIGHT / 2){
        point_height = SCREEN_HEIGHT / 2;
    }

    /* Calculate top-left position*/
    top = (SCREEN_HEIGHT / 2) - (point_height / 2);
    left = screen_x - (point_width / 2);

    /*Draw glowing base of Fortress Heart */
    DrawRectangle(
        (int)left,
        (int)top,
        (int)point_width,
        (int)point_height,
        GOLD
    );

    /* Draw center core to make it look like an objective marker*/
    DrawCircle(
        (int)screen_x,
        (int)(top + (point_height * 0.35f)),
        (int)(point_width * 0.45f),
        YELLOW
    );
}

 /**
  * render_game - Draws the current game feature
  * 
  * This function is responsible for rendering:
  * -The map (walls and floor)
  * -The player (as a red circle)
  * -Debug information (player position)
  * 
  * Rendering is done every frame inside the game loop.
  * 
  * Return: Nothing
  */
 static void render_game(void){
    float scale;
    int player_screen_x;
    int player_screen_y;
    int i;

    /* Convert world coordinates to rendered map coordinates*/
    scale = (float)MAP_RENDER_TILE / (float)TILE_SIZE;
    player_screen_x = MAP_OFFSET_X + (int)(player.x * scale);
    player_screen_y = MAP_OFFSET_Y + (int)(player.y * scale);


    BeginDrawing();/*Begin drawing a new frame*/

    /*Clear previous frame*/
    ClearBackground(BLACK);

    /* Render pseudo-3D wall view */
    raycast_render_3d(&player);

    /**
     * Sort enemies based on distacne from player.
     * This ensures correct rendering order:
     * farther enemies drawn first, closer ones last.
     */
    sort_enemies_by_distance(&player);

    /* Render all enemies in pseudo 3D view*/
    for(i = 0; i < MAX_ENEMIES; i++){
        enemy_render_3d(&enemies[i], &player, g_zbuffer);
    }

    /* Render Fortress Heart objective in pseudo-3D view*/
    render_control_point_3d();

    /**
     * Draw red screen flash when player takes damage.
     * Alpha is low so the game remins visible underneath.
     */
    if(damage_flash_timer > 0){
        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){255, 0, 0, 80});
    }

    /**
     * Draw a quick weapon swing flash.
     * This gives feedback that the player attacked.
     */
    if(attack_flash_timer > 0){
        DrawRectangle(
            (SCREEN_WIDTH / 2) - 40,
            (SCREEN_HEIGHT / 2) + 80,
            80,
            120,
            (Color){220, 220, 220, 120}
        );
    }

    /**
     * Draw attack result text.
     * HIT confirms enemy was defeated, MISS confirms attack happened
     * but no enemy was in range
     */
    if(attack_text_timer > 0){
        if(attack_result == 1){
            DrawText("HIT!", (SCREEN_WIDTH / 2) - 35, (SCREEN_HEIGHT / 2) - 80, 32, GREEN);
        }
        if(attack_result == 2){
            DrawText("MISS!", (SCREEN_WIDTH / 2) - 45, (SCREEN_HEIGHT / 2) - 80, 32, RAYWHITE);
        }
    }

    /* Title Area*/
    DrawText("Thimlich Hunt", 40, 25, 32, RAYWHITE);

    /* Render minimap only when enalbed*/
    if(show_minimap){
        /* Draw the map first */
        render_map();

        /**
        * Draw player as a simple circle(Awaiting 3D rendering)
        * 
        */
        DrawCircle(player_screen_x, player_screen_y, 7, RED);

        /* Draw all enemies on the minimap */
        for(i = 0; i < MAX_ENEMIES; i++){
            enemy_draw_minimap(&enemies[i]);
        }

        /**
         * Draw Fortress Heart on minimap only after enemies are defeated.
         * This guides the player to the reclaim objective.
         */
        if(control_point.active){
            DrawCircle(
                MAP_OFFSET_X + (int)(control_point.x * scale),
                MAP_OFFSET_Y + (int)(control_point.y * scale),
                6,
                GOLD);
        }

        /**
        * Draw  rays showing where the player is looking
        */
        raycast_draw_rays(&player);

    }
    
    /*Render the player health information*/
    DrawText(TextFormat("Health: %d", player.health), 40, 95, 20, RAYWHITE);

    /*Debug text */
    DrawText("Use arrow Keys to move | SPACE to attack", 40, 120, 20, LIGHTGRAY);

    /**
     * Show game-over overlay when player loses.
     * Gameplay updates are stopped in update_game().
     */
    if(game_state == GAME_OVER){
        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){0, 0, 0, 180});

        DrawText("GAME OVER", (SCREEN_WIDTH / 2) - 120, (SCREEN_HEIGHT / 2) - 30, 40, RED);
        DrawText("Press R to restart", (SCREEN_WIDTH / 2) - 110, (SCREEN_HEIGHT / 2) + 25, 24, RAYWHITE);
    }

    /**
     * Show victory overlay when all enemies are defeated.
     * Normal gameplay updates are paused in update_game().
     */
    if(game_state == GAME_VICTORY){
        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){0, 0, 0, 180});

        DrawText("CASTLE RECLAIMED", (SCREEN_WIDTH / 2) - 100, (SCREEN_HEIGHT / 2) - 40, 40, GREEN);
        DrawText("Press R to restart", (SCREEN_WIDTH / 2) - 110, (SCREEN_HEIGHT / 2) + 25, 24, RAYWHITE);
    }

   
    EndDrawing();
 }

 /**
  * game_run - Runs the main game loop
  * 
  * Return: Nothing
  */
 void game_run(void){
    /* Initialize player and enemy before starting the loop */
    reset_game();

    while(!WindowShouldClose()){
        process_input();
        update_game();
        render_game();
    }
 }
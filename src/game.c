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

    /**
     * Place enemies on different walkable map tiles.
     * These tile positons can be adjusted as the map evolves.
     */
    enemy_init(&enemies[0], 8, 8);
    enemy_init(&enemies[1], 7, 5);
    enemy_init(&enemies[2], 8, 7);

    damage_flash_timer = 0;
    damage_cooldown_timer = 0;
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
     * When player is dead, stop normal gameplay updates.
     * Pressing R restarts the game state.
     */
    if(player.health <= 0){
        if(IsKeyPressed(KEY_R)){
            reset_game();
        }
        return;
    }

    /* Update player (movement + colision)*/
    player_update(&player);

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

    /* Render all enemies in pseudo 3D view*/
    for(i = 0; i < MAX_ENEMIES; i++){
        enemy_render_3d(&enemies[i], &player, g_zbuffer);
    }

    /**
     * Draw red screen flash when player takes damage.
     * Alpha is low so the game remins visible underneath.
     */
    if(damage_flash_timer > 0){
        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){255, 0, 0, 80});
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
        * Draw  rays showing where the player is looking
        */
        raycast_draw_rays(&player);

    }
    
    /*Render the player health information*/
    DrawText(TextFormat("Health: %d", player.health), 40, 95, 20, RAYWHITE);

    /*Debug text */
    DrawText("Use arrow Keys to move", 40, 120, 20, LIGHTGRAY);
    DrawText(TextFormat("Player X: %.2f Y: %.2f", player.x, player.y), 40, 150, 20, LIGHTGRAY);

    /**
     * Show game-over message when player health reaches zero.
     * Gameplay updates are stopped in update_game().
     */
    if(player.health <= 0){
        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){0, 0, 0, 180});

        DrawText("GAME OVER", (SCREEN_WIDTH / 2) - 120, (SCREEN_HEIGHT / 2) - 30, 40, RED);
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
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
/* Global (module-local) enemy instance */
static Enemy enemy;

/* Tracks whether the minimap is visible */
static int show_minimap = 1;

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
  * update game - Updates the game state
  * 
  * Return: Nothing
  */
 static void update_game(void){
    /* AI and combat logic will be added here*/
    /* Update player (movement + colision)*/
    player_update(&player);

    /*Update enemy behavior*/
    enemy_update(&enemy, &player);

    /* Apply damage when enemy touches player*/
    if(enemy_check_player_contact(&enemy, &player) && player.health > 0){
        player.health -= 1;
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

    /* Convert world coordinates to rendered map coordinates*/
    scale = (float)MAP_RENDER_TILE / (float)TILE_SIZE;
    player_screen_x = MAP_OFFSET_X + (int)(player.x * scale);
    player_screen_y = MAP_OFFSET_Y + (int)(player.y * scale);


    BeginDrawing();/*Begin drawing a new frame*/

    /*Clear previous frame*/
    ClearBackground(BLACK);

    /* Render pseudo-3D wall view */
    raycast_render_3d(&player);

    /* Render pseudo 3D enemy*/
    enemy_render_3d(&enemy, &player, g_zbuffer);

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

        /* Draw enemy on minimap */
        enemy_draw_minimap(&enemy);

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

   
    EndDrawing();
 }

 /**
  * game_run - Runs the main game loop
  * 
  * Return: Nothing
  */
 void game_run(void){
    /* Initialize player before starting the loop */
    player_init(&player);

    /* Initalize player before starting the loop*/
    enemy_init(&enemy, 8, 8);

    while(!WindowShouldClose()){
        process_input();
        update_game();
        render_game();
    }
 }
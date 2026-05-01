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

#define MAP_RENDER_TILE 50
#define MAP_OFFSET_X 390
#define MAP_OFFSET_Y 100

 /* Global (module-local) player instance */
static Player player;

 /**
  * process_input - Handles player and system input
  * 
  * Return: Nothing
  * 
  */
 static void process_input(void){
    /* Input logic will be added here*/
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

    /* Title Area*/
    DrawText("Thimlich Hunt", 40, 25, 32, RAYWHITE);

    /* Draw the map first */
    render_map();

     /**
     * Draw player as a simple circle(Awaiting 3D rendering)
     * 
     */
    DrawCircle(player_screen_x, player_screen_y, 7, RED);

    /**
     * Draw a single ray showing where the player is looking
     */
    raycast_draw_rays(&player);


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

    while(!WindowShouldClose()){
        process_input();
        update_game();
        render_game();
    }
 }
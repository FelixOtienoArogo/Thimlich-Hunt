/**
 * @file game.c
 * @brief Main game loop and game-state orchestration.
 */

 #include "raylib.h"
 #include "game.h"


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
    /* Movement, collision, AI and combat logic will be added here*/
 }

 /**
  * render_game - Draws the current game feature
  * 
  * Return: Nothing
  */
 static void render_game(void){
    BeginDrawing();

    ClearBackground(BLACK);
    DrawText("Thimlich Hunt", 40, 40, 32, RAYWHITE);
    DrawText("Game loop abstraction is working", 40, 90, 20, LIGHTGRAY);

    EndDrawing();
 }

 /**
  * game_run - Runs the main game loop
  * 
  * Return: Nothing
  */
 void game_run(void){
    while(!WindowShouldClose()){
        process_input();
        update_game();
        render_game();
    }
 }
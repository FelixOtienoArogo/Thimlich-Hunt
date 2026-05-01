#ifndef PLAYER_H
#define PLAYER_H

/**
 * struct Player - Stores player position and movement state
 * @x: Player x-position in world coordinates (pixels)
 * @y: Player y-position in world coordinates (pixels)
 * @angle: Player viewing angle in degrees
 * @speed: Player movement speed
 * @rotation_speed: Player turning speed
 */
typedef struct Player
{
	float x;
	float y;
	float angle;
	float speed;
	float rotation_speed;
} Player;

/**
 * player_init - Initializes player values
 * @player: Pointer to player structure
 *
 * Return: Nothing
 */
void player_init(Player *player);

/**
 * player_update - Updates player movement and rotation
 * @player: Pointer to player structure
 *
 * Return: Nothing
 */
void player_update(Player *player);

#endif
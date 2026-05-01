#ifndef RAYCAST_H
#define RAYCAST_H

#include "player.h"

/**
 * raycast_draw_rays - Draws multiple rays (field of view)
 * @player: Pointer to the player structure
 * 
 * Return: Nothing
 * 
 */
void raycast_draw_rays(Player *player);


/**
 * raycast_render_3d - Renders pseudo-3D wall slices from raycasts
 * @player: Pointer to player structure
 * 
 * Return: Nothing
 */
void raycast_render_3d(Player *player);

#endif
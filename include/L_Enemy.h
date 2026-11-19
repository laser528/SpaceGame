#pragma once
#include "gf3d_vgraphics.h"
#include "L_Entity.h"
#include "L_gameTime.h"
#include "L_Player.h"


typedef struct
{
	int             health;
	float           speed;

	int				JumpStartTime;
	int             jumping;

	int				AbilityStartTime;
	int             ability;

	Entity*         enemyTarget;


}EnemyData;


int enemy_think(Entity* self);

Entity* Enemy_Pinwheel_load_from_file(const char* filename);

Entity* Enemy_Bracewheel_load_from_file(const char* filename);


Entity* Enemy_Flywheel_load_from_file(const char* filename);

Entity* Enemy_Soar_Wheel_load_from_file(const char* filename);

Entity* Enemy_Shockwheel_load_from_file(const char* filename);

#pragma once
#include "gfc_input.h"
#include "gf3d_vgraphics.h"
#include "L_Entity.h"
#include "L_gameTime.h"



typedef struct
{
	int             health;
	float           speed;
	
	int				JumpStartTime;
	int             jumping;

	int             dashStartTime;
	int             dashing;

	int             groundPound;

	GFC_Vector3D    cameraRotation;
	
}PlayerData;



Entity* Player_load_from_file(const char* filename);

int player_think(Entity* self);
int player_update(Entity* self);
#pragma once
#include "gfc_input.h"
#include "L_Entity.h"
#include "L_gameTime.h"



typedef struct
{
	int             health;
	int             speed;
	

	GFC_Vector3D    cameraRotation;
}PlayerData;



Entity* Player_load_from_file(const char* filename);

int player_think(Entity* self);
int player_update(Entity* self);
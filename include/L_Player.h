#pragma once
#include "gfc_input.h"
#include "L_Entity.h"



typedef struct
{
	int             health;
	int             speed;
	int             radius; // to create bounding box in gfc primatives

}PlayerData;



Entity* Player_load_from_file(const char* filename);

int player_think(Entity* self);
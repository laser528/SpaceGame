#pragma once

#include "gfc_input.h"
#include "L_Entity.h"
#include "L_gameTime.h"



typedef struct
{
	int             health;
	int             speed;
	int             gravityRadius; // to create bounding box in gfc primatives
	

}StructureData;



Entity* Structure_load_from_file(const char* filename, int scale, int gravityScale);



//int player_think(Entity* self);
//int player_update(Entity* self);
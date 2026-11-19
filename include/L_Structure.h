#pragma once

#include "gfc_input.h"
#include "gfc_primitives.h"
#include "L_Entity.h"
#include "gf3d_mesh.h"
#include "L_gameTime.h"



typedef struct
{
	int             health;
	int             speed;
	float           gravityRadius; // to create bounding box in gfc primatives
	
	int				AbilityStartTime;
	int             ability;

}StructureData;



Entity* Structure_load_from_file(const char* filename, float scale, float gravityScale);



//int player_think(Entity* self);
//int player_update(Entity* self);
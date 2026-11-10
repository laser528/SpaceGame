#include "gf3d_vgraphics.h"
#include "L_Player.h"

Entity* player = NULL;
PlayerData* pData = { 0 };

int player_think(Entity* self) {
	GFC_Matrix4* view = gf3d_vgraphics_get_view_matrix();

	if (gfc_input_key_down("RIGHT")) {

	}
	return 1;
}

Entity* Player_load_from_file(const char* filename) {
	Entity* player = Entity_load_from_file(filename);
	pData->radius = 5;
	player->data = pData;
	player->Think = player_think;
}
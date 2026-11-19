
//#include "L_Enemy.h"






//Entity* Enemy_Pinwheel_load_from_file(const char* filename) {
//	Entity* player = Entity_load_from_file(filename);
//
//	player->data = gfc_allocate_array(sizeof(EnemyData), 1);
//	EnemyData* pData = player->data;
//	if (!pData) {
//		return NULL;
//	}
//
//
//	pData->speed = 1;
//	pData->health = 1;
//
//	pData->enemyTarget = getPlayer();
//
//	player->radius = 7.0f;
//	player->Update = general_update;
//	player->Think = enemy_think;
//	player->Collision = NULL;
//	return player;
//}
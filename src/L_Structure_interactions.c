#pragma once
#include "L_Structure_interactions.h";


int boost_panel(Entity* self) {
	Entity* player = getPlayer();
	if (!self && !self->data) return;
	if (!player && !player->data) return;
	

	StructureData* selfData = (StructureData*)self->data;
	PlayerData* aData = (PlayerData*)player->data;


	if (gfc_vector3d_magnitude_between(self->translation, player->translation) < player->radius) {
		selfData->AbilityStartTime = SDL_GetTicks();
	}


	int time = SDL_GetTicks();


	if ((time - selfData->AbilityStartTime) < 3000) {
		selfData->ability = 1;
		aData->speed = 2;

	}
	if (time - selfData->AbilityStartTime > 3000) {
		selfData->ability = 0;
		aData->speed = 1;
	}


}

int tar_panel(Entity* self) {
	Entity* player = getPlayer();
	if (!self && !self->data) return;
	if (!player && !player->data) return;


	StructureData* selfData = (StructureData*)self->data;
	PlayerData* aData = (PlayerData*)player->data;


	if (gfc_vector3d_magnitude_between(self->translation, player->translation) < player->radius) {
		selfData->AbilityStartTime = SDL_GetTicks();
	}


	int time = SDL_GetTicks();


	if ((time - selfData->AbilityStartTime) < 3000) {
		selfData->ability = 1;
		aData->speed = 0.5;

	}
	if (time - selfData->AbilityStartTime > 3000) {
		selfData->ability = 0;
		aData->speed = 1;
	}


}
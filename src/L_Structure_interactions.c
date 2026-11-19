#pragma once
#include "L_Structure_interactions.h";


int boost_panel(Entity* self) {
	Entity* player = getPlayer();
	if (!self && !self->data) return;
	if (!player && !player->data) return;
	

	StructureData* selfData = (StructureData*)self->data;
	PlayerData* aData = (PlayerData*)player->data;


	if (gfc_vector3d_magnitude_between(self->translation, player->translation) < (player->radius + self->radius)) {
		selfData->AbilityStartTime = SDL_GetTicks();
	}


	int time = SDL_GetTicks();


	if ((time - selfData->AbilityStartTime) < 3000) {
		selfData->ability = 1;
		aData->speed *= 20;

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


	if (gfc_vector3d_magnitude_between(self->translation, player->translation) < (player->radius + self->radius)) {
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

int bounce_panel(Entity* self) {
	Entity* player = getPlayer();
	if (!self && !self->data) return;
	if (!player && !player->data) return;


	StructureData* selfData = (StructureData*)self->data;
	PlayerData* aData = (PlayerData*)player->data;
	GFC_Vector3D originalTranslation = self->translation;
	GFC_Vector3D movement;

	if (gfc_vector3d_magnitude_between(self->translation, player->translation) < (player->radius + self->radius)) {
		selfData->AbilityStartTime = SDL_GetTicks();
	}


	int time = SDL_GetTicks();


	double jumpProgress = 0;
	gfc_matrix4_from_vectors_q(player->view, player->translation, player->quaternion, player->scale);
	GFC_Vector3D up = { player->view[2][0], player->view[2][1], player->view[2][2] };
	GFC_Vector3D currentUp = gfc_vector3d_subbed(player->translation, *player->gravityPoint);
	gfc_vector3d_normalize(&up);
	gfc_vector3d_normalize(&currentUp);

	

	//gfc_vector3d_scale(movement, gravity, 1);
	//gfc_vector3d_add(self->translation, self->translation, movement);
	
	if ((time - selfData->AbilityStartTime) < 3000) {
		jumpProgress = (1 - (time - selfData->AbilityStartTime) * 0.002);

		gfc_vector3d_scale(movement, up, 1 + 2 * jumpProgress);
		gfc_vector3d_add(player->translation, player->translation, movement);

		if (check_collision(player)) {

			selfData->AbilityStartTime = 0;
			movement = gfc_vector3d(0, 0, 0);
			gfc_vector3d_scale(movement, up, player->radius + 0.15f);
			gfc_vector3d_add(movement, player->lastCollisionPoint, movement);
			player->translation = movement;
		}

	}



}


int Zero_G_panel(Entity* self) {
	Entity* player = getPlayer();
	if (!self && !self->data) return;
	if (!player && !player->data) return;


	StructureData* selfData = (StructureData*)self->data;
	PlayerData* aData = (PlayerData*)player->data;
	GFC_Vector3D originalTranslation = self->translation;
	GFC_Vector3D movement;

	if (gfc_vector3d_magnitude_between(self->translation, player->translation) < (player->radius + self->radius)) {
		selfData->AbilityStartTime = SDL_GetTicks();
	}


	int time = SDL_GetTicks();

	gfc_matrix4_from_vectors_q(player->view, player->translation, player->quaternion, player->scale);
	GFC_Vector3D up = { player->view[2][0], player->view[2][1], player->view[2][2] };
	GFC_Vector3D currentUp = gfc_vector3d_subbed(player->translation, *player->gravityPoint);
	gfc_vector3d_normalize(&up);
	gfc_vector3d_normalize(&currentUp);

	if ((time - selfData->AbilityStartTime) < 3000) {
		



		gfc_vector3d_scale(movement, up, 1);
		gfc_vector3d_add(player->translation, player->translation, movement);

		if (check_collision(player)) {
			selfData->AbilityStartTime = 0;
			movement = gfc_vector3d(0, 0, 0);
			gfc_vector3d_scale(movement, up, player->radius + 0.15f);
			gfc_vector3d_add(movement, player->lastCollisionPoint, movement);
			player->translation = movement;
		}

	}



}

int G_Switcher_panel(Entity* self) {
	Entity* player = getPlayer();
	if (!self && !self->data) return;
	if (!player && !player->data) return;


	StructureData* selfData = (StructureData*)self->data;
	PlayerData* aData = (PlayerData*)player->data;
	GFC_Vector3D originalTranslation = self->translation;
	GFC_Vector3D movement;

	if (gfc_vector3d_magnitude_between(self->translation, player->translation) < (player->radius + self->radius)) {
		selfData->AbilityStartTime = SDL_GetTicks();
	}


	int time = SDL_GetTicks();


	double jumpProgress = 0;



	if (time - selfData->AbilityStartTime < 1000) {
		selfData->ability = 1;
	}

	gfc_matrix4_from_vectors_q(player->view, player->translation, player->quaternion, player->scale);
	GFC_Vector3D up = { player->view[2][0], player->view[2][1], player->view[2][2] };
	GFC_Vector3D currentUp = gfc_vector3d_subbed(player->translation, *player->gravityPoint);
	GFC_Vector3D forward = { -self->view[1][0], -self->view[1][1], -self->view[1][2] };
	gfc_vector3d_normalize(&forward);
	gfc_vector3d_normalize(&up);
	gfc_vector3d_normalize(&currentUp);

	if (selfData->ability) {
		selfData->ability = 0;
		gfc_vector3d_scale(movement, forward, 10.0f);
		gfc_vector3d_add(player->translation, player->translation, movement);


		if (check_collision(player)) {
			
			movement = gfc_vector3d(0, 0, 0);
			gfc_vector3d_scale(movement, up, player->radius + 0.15f);
			gfc_vector3d_add(movement, player->lastCollisionPoint, movement);
			player->translation = movement;
		}

	}




}

#pragma once
#include "L_Enemy.h"

GFC_Vector3D Enemy_Gravity(Entity* self) {
	GFC_Vector3D originalTranslation = self->translation;
	GFC_Vector3D originalScale = self->scale;
	GFC_Vector4D originalQuterion = self->quaternion;
	EnemyData* selfData = (EnemyData*)self->data;
	GFC_Vector3D movement;

	if (self->gravityPoint == NULL) return gfc_vector3d(0, 0, 1);

	int time = SDL_GetTicks();

	GFC_Vector3D gravity = gfc_vector3d_subbed(*self->gravityPoint, self->translation);
	GFC_Vector3D up = gfc_vector3d_subbed(self->translation, *self->gravityPoint);

	gfc_vector3d_normalize(&gravity);
	gfc_vector3d_normalize(&up);

	gfc_matrix4_from_vectors_q(self->view, self->translation, self->quaternion, self->scale);

	GFC_Vector3D currentUp = {self->view[2][0], self->view[2][1], self->view[2][2]};

	gfc_quaternion_align_up(&self->quaternion, currentUp, up);

	gfc_vector3d_scale(movement, gravity, 1);
	gfc_vector3d_add(self->translation, self->translation, movement);

	if (check_collision(self)) {
		movement = gfc_vector3d(0, 0, 0);
		gfc_vector3d_scale(movement, up, self->radius + 0.15f);
		gfc_vector3d_add(movement, self->lastCollisionPoint, movement);
		self->translation = movement;
	}

	return up;
}


int enemy_think(Entity* self) {
	EnemyData* selfData = (EnemyData*)self->data;
	GFC_Vector3D movement = { 0 };

	GFC_Vector3D planetUp = Enemy_Gravity(self);

	GFC_Vector3D originalTranslation = self->translation;
	GFC_Vector3D originalScale = self->scale;
	GFC_Vector4D originalQuterion = self->quaternion;

	float speed = selfData->speed;


	Entity* target = selfData->enemyTarget;
	if (!target) return;

	Entity_look_at(self, target);

	gfc_matrix4_from_vectors_q(self->view, self->translation, self->quaternion, self->scale);
	GFC_Vector3D forward = { -self->view[1][0], -self->view[1][1], -self->view[1][2] };
	GFC_Vector3D up = { self->view[2][0], self->view[2][1], self->view[2][2] };
	
	gfc_vector3d_normalize(&forward);
	gfc_vector3d_scale(movement, forward, speed);
	gfc_vector3d_add(self->translation, self->translation, movement);


	if (check_collision(self)) {
		//self->quaternion = originalQuterion;

		self->translation = originalTranslation;
		return 0;
	}


	return 1;
}

int enemy_update(Entity* self) {
	general_update(self);

}

int enemy_Attacked(Entity* self, Entity* Attacker) {
	if (!self && !self->data) return;
	EnemyData* pData = (EnemyData*)self->data;
	
	pData->health -= 1;
	if (pData->health < 1) {
		Entity_free(self);
	}
}

int enemy_Bracewheel_Attacked(Entity* self, Entity* Attacker) {
	if (!self && !self->data) return;
	if (!Attacker && !Attacker->data) return;
	if (Attacker->team != PLAYER_TEAM) return;

	EnemyData* pData = (EnemyData*)self->data;
	PlayerData* aData = (PlayerData*)Attacker->data;

	if (aData->groundPound) {
		pData->health -= 1;
	}

	if (pData->health < 1) {
		Entity_free(self);
	}
}

int enemy_Bracewheel_think(Entity* self) {
	Enemy_Gravity(self);
}

int Enemy_free(Entity* self) {
	if (!self && !self->data) return;
	EnemyData* pData = (EnemyData*)self->data;
	free(pData);
	self->free = NULL;
}



int enemy_fly_Wheel_think(Entity* self) {
	EnemyData* selfData = (EnemyData*)self->data;
	GFC_Vector3D movement = { 0 };

	GFC_Vector3D planetUp = Enemy_Gravity(self);

	GFC_Vector3D originalTranslation = self->translation;
	GFC_Vector3D originalScale = self->scale;
	GFC_Vector4D originalQuterion = self->quaternion;

	float speed = selfData->speed;

	int time = SDL_GetTicks();

	Entity* target = selfData->enemyTarget;
	if (!target) return;

	Entity_look_at(self, target);

	gfc_matrix4_from_vectors_q(self->view, self->translation, self->quaternion, self->scale);
	GFC_Vector3D forward = { -self->view[1][0], -self->view[1][1], -self->view[1][2] };
	GFC_Vector3D up = { self->view[2][0], self->view[2][1], self->view[2][2] };

	gfc_vector3d_normalize(&up);



	double jumpProgress = 0;

	if (time - selfData->JumpStartTime < 500) {
		jumpProgress = (1 - (time - selfData->JumpStartTime) * 0.002);

		gfc_vector3d_scale(movement, up, 1 + 2 * jumpProgress);
		gfc_vector3d_add(self->translation, self->translation, movement);
		if (check_collision(self)) {
			self->translation = originalTranslation;
		}

	}
	if (time - selfData->JumpStartTime > 1000) {
		selfData->JumpStartTime = SDL_GetTicks();
	}

	return 1;
}

int enemy_fly_Wheel_Attacked(Entity* self, Entity* Attacker) {
	if (!self && !self->data) return;
	if (!Attacker && !Attacker->data) return;
	if (Attacker->team != PLAYER_TEAM) return;

	EnemyData* pData = (EnemyData*)self->data;
	PlayerData* aData = (PlayerData*)Attacker->data;

	if (aData->jumping > 0) {
		aData->jumping -= 1;
	}

	

	if (pData->health < 1) { // dosent die on attack
		Entity_free(self);
	}
}
int enemy_shock_Wheel_Attacked(Entity* self, Entity* Attacker) {
	if (!self && !self->data) return;
	if (!Attacker && !Attacker->data) return;
	if (Attacker->team != PLAYER_TEAM) return;

	EnemyData* pData = (EnemyData*)self->data;
	PlayerData* aData = (PlayerData*)Attacker->data;

	if (pData->ability) {
		return 0;
	}

	if (aData->jumping > 0) {
		aData->jumping -= 1;
	}



	if (pData->health < 1) { // dosent die on attack
		Entity_free(self);
	}
}

int enemy_soar_Wheel_Attacked(Entity* self, Entity* Attacker) {
	if (!self && !self->data) return;
	if (!Attacker && !Attacker->data) return;
	if (Attacker->team != PLAYER_TEAM) return;

	EnemyData* pData = (EnemyData*)self->data;
	PlayerData* aData = (PlayerData*)Attacker->data;

	if (aData->jumping > 0) {
		aData->jumping -= 1;
	}
	
	pData->health -= 1;

	if (pData->health < 1) { // dosent die on attack
		Entity_free(self);
	}
}

Entity* Enemy_Pinwheel_load_from_file(const char* filename) {
	Entity* player = Entity_load_from_file(filename);

	player->data = gfc_allocate_array(sizeof(EnemyData), 1);
	EnemyData* pData = (EnemyData*)player->data;
	if (!pData) {
		return NULL;
	}


	pData->speed = 0.5;
	pData->health = 1;

	pData->enemyTarget = getPlayer();

	player->radius = 7.0f;
	player->Update = enemy_update;
	player->Think = enemy_think;
	player->Attacked = enemy_Attacked;
	player->Collision = NULL;
	player->free = Enemy_free;
	player->team = 2;
	return player;
}

int enemy_soar_wheel_think(Entity* self) {
	EnemyData* selfData = (EnemyData*)self->data;
	GFC_Vector3D movement = { 0 };

	GFC_Vector3D originalTranslation = self->translation;

	GFC_Vector3D planetUp = Enemy_Gravity(self);

	self->translation = originalTranslation;
	GFC_Vector3D originalScale = self->scale;
	GFC_Vector4D originalQuterion = self->quaternion;

	float speed = selfData->speed;


	Entity* target = selfData->enemyTarget;
	if (!target) return;

	Entity_look_at(self, target);

	gfc_matrix4_from_vectors_q(self->view, self->translation, self->quaternion, self->scale);
	GFC_Vector3D forward = gfc_vector3d_subbed(target->translation, self->translation);
	GFC_Vector3D up = { self->view[2][0], self->view[2][1], self->view[2][2] };

	gfc_vector3d_normalize(&forward);
	gfc_vector3d_scale(movement, forward, speed);
	gfc_vector3d_add(self->translation, self->translation, movement);


	if (check_collision(self)) {
		movement = gfc_vector3d(0, 0, 0);
		gfc_vector3d_scale(movement, up, self->radius + 0.15f);
		gfc_vector3d_add(movement, self->lastCollisionPoint, movement);
		self->translation = movement;
	}


	return 1;
}



int enemy_shock_Wheel_think(Entity* self) {
	EnemyData* selfData = (EnemyData*)self->data;
	GFC_Vector3D movement = { 0 };

	GFC_Vector3D planetUp = Enemy_Gravity(self);

	GFC_Vector3D originalTranslation = self->translation;
	GFC_Vector3D originalScale = self->scale;
	GFC_Vector4D originalQuterion = self->quaternion;

	float speed = selfData->speed;

	int time = SDL_GetTicks();

	Entity* target = selfData->enemyTarget;
	if (!target) return;

	Entity_look_at(self, target);

	gfc_matrix4_from_vectors_q(self->view, self->translation, self->quaternion, self->scale);
	GFC_Vector3D forward = { -self->view[1][0], -self->view[1][1], -self->view[1][2] };
	GFC_Vector3D up = { self->view[2][0], self->view[2][1], self->view[2][2] };

	gfc_vector3d_normalize(&up);



	double jumpProgress = 0;

	if (time - selfData->JumpStartTime < 500) {
		jumpProgress = (1 - (time - selfData->JumpStartTime) * 0.002);

		gfc_vector3d_scale(movement, up, 1 + 2 * jumpProgress);
		gfc_vector3d_add(self->translation, self->translation, movement);
		if (check_collision(self)) {
			self->translation = originalTranslation;
		}

	}
	if (time - selfData->JumpStartTime > 1000) {
		selfData->JumpStartTime = SDL_GetTicks();
	}

	if ((time - selfData->AbilityStartTime) > 3000 && (time - selfData->AbilityStartTime) < 6000) {
		selfData->ability = 1;
		self->colorMod = GFC_COLOR_BLUE;

	}
	if (time - selfData->AbilityStartTime > 6000) {
		selfData->AbilityStartTime = SDL_GetTicks();
		self->colorMod = GFC_COLOR_WHITE;
		selfData->ability = 0;
	}


	return 1;
}


Entity* Enemy_Bracewheel_load_from_file(const char* filename) {
	Entity* player = Entity_load_from_file(filename);

	player->data = gfc_allocate_array(sizeof(EnemyData), 1);
	EnemyData* pData = (EnemyData*)player->data;
	if (!pData) {
		return NULL;
	}

	player->colorMod = GFC_COLOR_MAGENTA;

	pData->speed = 0.5;
	pData->health = 1;

	pData->enemyTarget = getPlayer();

	player->radius = 7.0f;
	player->Update = enemy_update;
	player->Think = enemy_Bracewheel_think;
	player->Attacked = enemy_Bracewheel_Attacked;
	player->Collision = NULL;
	player->free = Enemy_free;
	player->team = 2;
	return player;
}

Entity* Enemy_Flywheel_load_from_file(const char* filename) {
	Entity* player = Entity_load_from_file(filename);

	player->data = gfc_allocate_array(sizeof(EnemyData), 1);
	EnemyData* pData = (EnemyData*)player->data;
	if (!pData) {
		return NULL;
	}

	player->colorMod = GFC_COLOR_LIGHTGREEN;

	pData->speed = 0.5;
	pData->health = 1;

	pData->enemyTarget = getPlayer();

	player->radius = 7.0f;
	player->Update = enemy_update;
	player->Think = enemy_fly_Wheel_think;
	player->Attacked = enemy_fly_Wheel_Attacked;
	player->Collision = NULL;
	player->free = Enemy_free;
	player->team = 2;
	return player;
}

Entity* Enemy_Soar_Wheel_load_from_file(const char* filename) {
	Entity* player = Entity_load_from_file(filename);

	player->data = gfc_allocate_array(sizeof(EnemyData), 1);
	EnemyData* pData = (EnemyData*)player->data;
	if (!pData) {
		return NULL;
	}

	player->colorMod = GFC_COLOR_DARKGREEN;

	pData->speed = 0.5;
	pData->health = 1;

	pData->enemyTarget = getPlayer();

	player->radius = 7.0f;
	player->Update = enemy_update;
	player->Think = enemy_soar_wheel_think;
	player->Attacked = enemy_soar_Wheel_Attacked;
	player->Collision = NULL;
	player->free = Enemy_free;
	player->team = 2;
	return player;
}

Entity* Enemy_Shockwheel_load_from_file(const char* filename) {
	Entity* player = Entity_load_from_file(filename);

	player->data = gfc_allocate_array(sizeof(EnemyData), 1);
	EnemyData* pData = (EnemyData*)player->data;
	if (!pData) {
		return NULL;
	}

	player->colorMod = GFC_COLOR_WHITE;

	pData->speed = 0.5;
	pData->health = 1;

	pData->enemyTarget = getPlayer();

	player->radius = 7.0f;
	player->Update = enemy_update;
	player->Think = enemy_shock_Wheel_think;
	player->Attacked = enemy_shock_Wheel_Attacked;
	player->Collision = NULL;
	player->free = Enemy_free;
	player->team = 2;
	return player;
}
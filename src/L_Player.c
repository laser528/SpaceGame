
#include "L_Player.h"

Entity* Player = NULL;

Entity* getPlayer() {
	return Player;
}

GFC_Vector3D gameWorldExtents = {500};


void camera_Update(Entity* self) {
	GFC_Matrix4* view = gf3d_vgraphics_get_view_matrix();
	GFC_Vector3D offset = { 0 };
	GFC_Vector3D rotatedOffset = { 0 };
	GFC_Vector3D heightOffset = { 0 };
	GFC_Vector3D cameraPosition;
	PlayerData* selfData = self->data;

	gfc_matrix4_from_vectors_q(self->view, self->translation, self->quaternion, self->scale);


	GFC_Vector3D forward = {-self->view[1][0], -self->view[1][1], -self->view[1][2]};
	GFC_Vector3D right = {self->view[0][0], self->view[0][1], self->view[0][2]};
	GFC_Vector3D up = {self->view[2][0], self->view[2][1], self->view[2][2]};

	gfc_vector3d_normalize(&forward);
	gfc_vector3d_normalize(&right);
	gfc_vector3d_normalize(&up);


	gfc_vector3d_scale(offset, forward, -40);
	gfc_vector3d_scale(heightOffset, up, +30);

	//gfc_vector3d_scale(offset, forward, -20);
	//gfc_vector3d_scale(heightOffset, up, +10);

	gfc_vector3d_add(offset, offset, heightOffset);


	rotate_vector_around_axis(&offset, up, offset, selfData->cameraRotation.x);

	gfc_vector3d_add(cameraPosition, self->translation, offset);

	gfc_matrix4_view(
		*view,
		cameraPosition,
		self->translation,
		up
	);
}

int player_update(Entity* self) {
	camera_Update(self);
	general_update(self);

}

GFC_Vector3D Player_Gravity(Entity* self) {
	GFC_Vector3D originalTranslation = self->translation;
	GFC_Vector3D originalScale = self->scale;
	GFC_Vector4D originalQuterion = self->quaternion;
	PlayerData* selfData = self->data;
	GFC_Vector3D movement;

	if (self->gravityPoint == NULL) return gfc_vector3d(0, 0, 1);

	int time = SDL_GetTicks();
	
	GFC_Vector3D gravity = gfc_vector3d_subbed(*self->gravityPoint, self->translation);
	GFC_Vector3D up = gfc_vector3d_subbed(self->translation, *self->gravityPoint);
	
	gfc_vector3d_normalize(&gravity);
	gfc_vector3d_normalize(&up);

	gfc_matrix4_from_vectors_q(self->view, self->translation, self->quaternion, self->scale);

	GFC_Vector3D currentUp = {
		self->view[2][0],
		self->view[2][1],
		self->view[2][2]
	};

	gfc_quaternion_align_up(&self->quaternion, currentUp, up);

	gfc_vector3d_scale(movement, gravity, 1);
	gfc_vector3d_add(self->translation, self->translation, movement);

	if (check_collision(self)) {

		movement = gfc_vector3d(0,0,0);
		gfc_vector3d_scale(movement, up, self->radius+0.15f);
		gfc_vector3d_add(movement, self->lastCollisionPoint, movement);
		self->translation = movement;
		
		selfData->jumping = 0;
		selfData->groundPound = 0;

		if (time - selfData->dashStartTime > 2500) {
			selfData->dashing = 0;
		}
	}

	return up;
}

void player_Actions(Entity* self) {
	PlayerData* selfData = self->data;
	GFC_Vector3D movement;

	GFC_Vector3D originalTranslation = self->translation;
	GFC_Vector3D originalScale = self->scale;
	GFC_Vector4D originalQuterion = self->quaternion;

	int time = SDL_GetTicks();
	double jumpProgress = 0;
	double dashProgress = 0;

	gfc_matrix4_from_vectors_q(self->view, self->translation, self->quaternion, self->scale);

	GFC_Vector3D up = { self->view[2][0], self->view[2][1], self->view[2][2] };
	GFC_Vector3D forward = { -self->view[1][0], -self->view[1][1], -self->view[1][2] };
	gfc_vector3d_normalize(&forward);
	gfc_vector3d_normalize(&up);

	// Jump and Double jump
	movement = gfc_vector3d(0, 0, 0);
	if (gfc_input_key_down("SPACE")) {
		if (selfData->jumping < 2 && (time - selfData->JumpStartTime) > 250) {
			selfData->jumping += 1;
			selfData->JumpStartTime = SDL_GetTicks();
		}
	}
	if (time - selfData->JumpStartTime < 500) {
		jumpProgress = (1 - (time - selfData->JumpStartTime) * 0.002);
		
		if (selfData->jumping > 1) {
			check_attack(self, 14.0f, ENEMY_TEAM);
		}

		gfc_vector3d_scale(movement, up, 1 + 2 * jumpProgress);
		gfc_vector3d_add(self->translation, self->translation, movement);
		if (check_collision(self)) {
			self->translation = originalTranslation;
		}

	}

	// Dash 
	movement = gfc_vector3d(0, 0, 0);
	if (gfc_input_key_down("q")) {
		if (!selfData->dashing) {
			selfData->dashing = 1;
			selfData->dashStartTime = SDL_GetTicks();
		}
	}
	if (time - selfData->dashStartTime < 500) {
		dashProgress = (1 - (time - selfData->dashStartTime) * 0.002);
		
		
		gfc_vector3d_scale(movement, forward, 1 + 2 * dashProgress);
		gfc_vector3d_add(self->translation, self->translation, movement);
		

		check_attack(self, 14.0f, ENEMY_TEAM);
		
		if (check_collision(self)) {
			movement = gfc_vector3d(0, 0, 0);
			gfc_vector3d_scale(movement, up, self->radius + 0.5f);
			gfc_vector3d_add(self->translation, self->translation, movement);

			/*self->translation = originalTranslation;*/
		}

	}

	// Ground Pound
	movement = gfc_vector3d(0, 0, 0);
	if (gfc_input_key_down("LCTRL")) {
		if (!selfData->groundPound) {
			selfData->groundPound = 1;
		}
	}
	if (selfData->groundPound) {
		GFC_Vector3D down = { 0 };
		gfc_vector3d_negate(down, up);

		gfc_vector3d_scale(movement, down, 3);
		gfc_vector3d_add(self->translation, self->translation, movement);
		if (check_collision(self)) {
			self->translation = originalTranslation;
		}
		check_attack(self, 14.0f, ENEMY_TEAM);

	}

}

int player_think(Entity* self) {
	PlayerData* selfData = self->data;
	GFC_Vector3D movement;
	
	GFC_Vector3D originalTranslation = self->translation;
	GFC_Vector3D originalScale = self->scale;
	GFC_Vector4D originalQuterion = self->quaternion;

	gfc_matrix4_from_vectors_q(self->view, self->translation, self->quaternion, self->scale);

	GFC_Vector3D planetUp = Player_Gravity(self);

	

	player_Actions(self);
	
	// Player input
	if (gfc_input_key_down("RIGHT")) {
		selfData->cameraRotation.x += 1;
	}
	if (gfc_input_key_down("LEFT")) {
		selfData->cameraRotation.x -= 1;
	}
	if (gfc_input_key_down("UP")) {
		GFC_Vector3D up = { self->view[2][0], self->view[2][1], self->view[2][2] };
		float speed = selfData->speed;
		// Run
		if (gfc_input_key_down("LSHIFT")) {
			speed *= 2;
		}
		gfc_vector4d_rotate_world_by_degrees(&self->quaternion, planetUp, selfData->cameraRotation.x);

		selfData->cameraRotation.x *= 0.25;

		gfc_matrix4_from_vectors_q(self->view, self->translation, self->quaternion, self->scale);
		GFC_Vector3D forward = { -self->view[1][0], -self->view[1][1], -self->view[1][2] };
		gfc_vector3d_normalize(&forward);
		
		
		gfc_vector3d_scale(movement, forward, speed);
		gfc_vector3d_add(self->translation, self->translation, movement);
		
		if (check_collision(self)) {
			self->translation = originalTranslation;
			return 0;
		}
			
		return 1;

	}
	if (gfc_input_key_down("DOWN")) {

		GFC_Vector3D up = { self->view[2][0], self->view[2][1], self->view[2][2] };
		gfc_vector4d_rotate_world_by_degrees(&self->quaternion, planetUp, selfData->cameraRotation.x);
		selfData->cameraRotation.x *= 0.25;

		gfc_matrix4_from_vectors_q(self->view, self->translation, self->quaternion, self->scale);
		GFC_Vector3D forward = { self->view[1][0], self->view[1][1], self->view[1][2] };
		gfc_vector3d_normalize(&forward);

		
		gfc_vector3d_scale(movement, forward, selfData->speed);
		gfc_vector3d_add(self->translation, self->translation, movement);

		if (check_collision(self)) {
			self->translation = originalTranslation;
			return 0;
		}

		return 1;
	}
	
	return 1;
}



Entity* Player_load_from_file(const char* filename) {
	Entity* player = Entity_load_from_file(filename);

	player->data = gfc_allocate_array(sizeof(PlayerData), 1);
	PlayerData* pData = player->data;
	if (!pData) {
		slog("Killer");
		return NULL;
	}

	pData->cameraRotation = gfc_vector3d(0, 0, 0);
	pData->speed = 1;

	pData->JumpStartTime = 0;
	pData->jumping = 0;

	player->radius = 7.0f;
	player->Update = player_update;
	player->Think = player_think;
	player->Collision = NULL;
	player->Attacked = NULL;
	player->team = 1;
	Player = player;

	return player;
}
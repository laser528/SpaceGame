
#include "L_Player.h"

Entity* player = NULL;

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


	gfc_vector3d_scale(offset, forward, -20);
	gfc_vector3d_scale(heightOffset, up, +10);

	gfc_vector3d_add(offset, offset, heightOffset);


	rotate_vector_around_axis(&offset, up, offset, selfData->cameraRotation.x);

	gfc_vector3d_add(cameraPosition, self->translation, offset);

	gfc_matrix4_view(
		*view,
		cameraPosition,
		self->translation,
		gfc_vector3d(0, 0, 1)
	);
}

int player_update(Entity* self) {
	camera_Update(self);
	general_update(self);

}

int Player_Gravity(Entity* self) {
	GFC_Vector3D originalTranslation = self->translation;
	GFC_Vector3D originalScale = self->scale;
	GFC_Vector4D originalQuterion = self->quaternion;
	PlayerData* selfData = self->data;
	GFC_Vector3D movement;

	GFC_Vector3D gravity = gfc_vector3d_subbed(*self->gravityPoint, self->translation);
	GFC_Vector3D up = gfc_vector3d_subbed(*self->gravityPoint, self->translation);

	gfc_vector3d_normalize(&gravity);
	gfc_vector3d_normalize(&up);

	gfc_vector4d_rotate_by_degrees(&self->quaternion, up, selfData->cameraRotation.x);

	gfc_vector3d_scale(movement, gravity, 1);
	gfc_vector3d_add(self->translation, self->translation, gravity);

	if (check_collision(self)) {
		self->translation = originalTranslation;
		return 0;
	}
	return 1;
}

int player_think(Entity* self) {
	PlayerData* selfData = self->data;
	GFC_Vector3D movement;
	
	GFC_Vector3D originalTranslation = self->translation;
	GFC_Vector3D originalScale = self->scale;
	GFC_Vector4D originalQuterion = self->quaternion;


	gfc_matrix4_from_vectors_q(self->view, self->translation, self->quaternion, self->scale);


	// Player input
	if (gfc_input_key_down("RIGHT")) {
		selfData->cameraRotation.x += 1;
	}
	if (gfc_input_key_down("LEFT")) {
		selfData->cameraRotation.x -= 1;
	}
	if (gfc_input_key_down("UP")) {
		GFC_Vector3D up = { self->view[2][0], self->view[2][1], self->view[2][2] };

		gfc_vector4d_rotate_by_degrees(&self->quaternion, up, selfData->cameraRotation.x);

		selfData->cameraRotation.x = 0;

		gfc_matrix4_from_vectors_q(self->view, self->translation, self->quaternion, self->scale);
		GFC_Vector3D forward = { -self->view[1][0], -self->view[1][1], -self->view[1][2] };
		gfc_vector3d_normalize(&forward);
		
		
		gfc_vector3d_scale(movement, forward, selfData->speed);
		gfc_vector3d_add(self->translation, self->translation, movement);


		if (check_collision(self)) {
			self->translation = originalTranslation;
			return 0;
		}
			


		return 1;

	}
	if (gfc_input_key_down("DOWN")) {

		GFC_Vector3D up = { self->view[2][0], self->view[2][1], self->view[2][2] };
		gfc_vector4d_rotate_by_degrees(&self->quaternion, up, selfData->cameraRotation.x);
		selfData->cameraRotation.x = 0;

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
	player->radius = 5.0f;

	player->Update = player_update;
	player->Think = player_think;
	player->Collision = NULL;
}
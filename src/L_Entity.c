
#include "L_Entity.h"

typedef struct {
	Entity* entityList;
	Uint32 entityCount;
}EntityManager;

static EntityManager entity_manager = { 0 };


int check_collision(Entity* self) {
	int i;
	Entity ent;
	
	for (i = 0; i < entity_manager.entityCount; i++) {
		ent = entity_manager.entityList[i];
		if (ent._refCount < 1) { continue; }
		if (ent.Collision != NULL) {
			slog("start COllision %i", i);
			if (ent.Collision(&entity_manager.entityList[i], self)) return 1;
		}

	}
	return 0;
}

// General entity update 
void general_update(Entity* self) {
	gfc_matrix4_from_vectors_q(self->view, self->translation, self->quaternion, self->scale);
	gf3d_model_queue_render(self->model, self->view, self->colorMod);
}

void Entity_Think() {
	int i;
	Entity ent;
	slog("agus Think");


	for (i = 0; i < entity_manager.entityCount; i++) {
		ent = entity_manager.entityList[i];
		if (ent._refCount < 1) { continue; }
		if (ent.Think != NULL) { 
			ent.Think(&entity_manager.entityList[i]);
		}
		
	}

}

void Entity_Update() {
	int i;
	Entity ent;
	
	slog("agus Update");

	for (i = 0; i < entity_manager.entityCount; i++) {
		ent = entity_manager.entityList[i];
		if (ent._refCount < 1) { continue; }
		if (ent.Update != NULL) {
			
			ent.Update(&entity_manager.entityList[i]);
		}
		/*gfc_matrix4_from_vectors(ent.view, gfc_vector3d(0, 0, 0), gfc_vector3d(0, 0, 0), gfc_vector3d(1, 1, 1));*/
		//gfc_matrix4_from_vectors_q(ent.view, ent.translation, ent.quaternion, ent.scale);
		//gf3d_model_queue_render(ent.model, ent.view, ent.colorMod);
	}

}

Entity* Entity_load_from_file(const char* filename) {
	Entity* ent = Entity_new();
	if (!ent) {
		slog("Entity failed to load");
		return NULL;
	}
	Model* mod = gf3d_model_load_from_file(filename);
	if (!mod) {
		slog("Entity not found from file");
		Entity_free(ent);
		return NULL;
	}
	ent->model = mod;
	ent->Update = general_update;
	ent->_refCount += 1;
	return ent;
}

void Entity_init_system(Uint32 entityCount) {
	if (entity_manager.entityCount != 0) {
		slog("Cannot init Model system, already initilized");
		return;
	}
	if (entityCount == 0) {
		slog("Cannot initalize Model system for ZERO Models");
		return;
	}
	entity_manager.entityCount = entityCount;
	entity_manager.entityList = gfc_allocate_array(sizeof(Entity), entityCount);

	for (int i = 0; i < entityCount; i++) { 
		entity_manager.entityList[i].id = i; 
		entity_manager.entityList[i].scale = gfc_vector3d(1,1,1);
		entity_manager.entityList[i].quaternion = gfc_vector4d(0, 0, 0, 1);
		entity_manager.entityList[i].colorMod = GFC_COLOR_WHITE;
		entity_manager.entityList[i].Update = NULL;
		entity_manager.entityList[i].Think = NULL;
		entity_manager.entityList[i].Collision = NULL;
		entity_manager.entityList[i]._refCount = 0;
	}
	model_init_system(entityCount);
	atexit(gf3d_model_close);
}

Entity* Entity_new() {
	int i;
	slog("Attempt to make Entity");
	for (i = 0; i < entity_manager.entityCount; i++) {
		if (entity_manager.entityList[i]._refCount <= 0) {
			slog("Entity index %i", i);
			entity_manager.entityList[i]._refCount += 1;

			
			return &entity_manager.entityList[i];
		}
	}

	return NULL;
}

void Entity_free(Entity* self) {
	if (!self)return;
	self->_refCount--;
	if (self->data) free(self->data);
}

void Entity_close() {
	memset(&entity_manager, 0, sizeof(entity_manager));
	free(entity_manager.entityList);
}
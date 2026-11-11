
#include "L_Entity.h"

typedef struct {
	Entity* entityList;
	Uint32 entityCount;
}EntityManager;

static EntityManager entity_manager = { 0 };

void Entity_Think() {
	int count = gfc_list_get_count(entity_manager.entityList);
	int i;
	Entity ent;
	


	for (i = 0; i < count; i++) {
		ent = entity_manager.entityList[i];
		if (ent._refCount < 1) { continue; }
		if (ent.Think != NULL) { 
			ent.Think(&ent); 
		}
		
	}

}

void Entity_Update() {
	int count = gfc_list_get_count(entity_manager.entityList);
	int i;
	Entity ent;
	
	

	for (i = 0; i < count; i++) {
		ent = entity_manager.entityList[i];
		if (ent._refCount < 1) { continue; }
		
		/*gfc_matrix4_from_vectors(ent.view, gfc_vector3d(0, 0, 0), gfc_vector3d(0, 0, 0), gfc_vector3d(1, 1, 1));*/
		gfc_matrix4_from_vectors_q(ent.view, ent.translation, ent.quaternion, ent.scale);
		gf3d_model_queue_render(ent.model, ent.view, ent.colorMod);
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
	ent->scale = gfc_vector3d(1, 1, 1);
	ent->quaternion = gfc_vector4d(0, 0, 0, 1);
	ent->colorMod = GFC_COLOR_WHITE;
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
		entity_manager.entityList[i].colorMod = GFC_COLOR_WHITE;
	}
	model_init_system(entityCount);
	atexit(gf3d_model_close);
}

Entity* Entity_new() {
	int i;
	slog("Attempt to make model %i", entity_manager.entityCount);
	for (i = 0; i < entity_manager.entityCount; i++) {
		slog("Model ref count %i", entity_manager.entityList[i]._refCount);
		if (entity_manager.entityList[i]._refCount <= 0) {
			entity_manager.entityList[i]._refCount += 1;

			entity_manager.entityCount += 1;
			return &entity_manager.entityList[i];
		}
	}

	return NULL;
}

void Entity_free(Entity* self) {
	if (!self)return;
	self->_refCount--;
}

void Entity_close() {
	memset(&entity_manager, 0, sizeof(entity_manager));
	free(entity_manager.entityList);
}
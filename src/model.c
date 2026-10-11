#include "simple_logger.h"

#include "gfc_types.h"


#include "model.h"




typedef struct {
	Model* modelList;
	Uint32 modelCount;
	Pipeline* pipe;
	VkDevice device;
}ModelManager;


/*
Model initi
model new
model load
model delete
model render
*/

static ModelManager model_manager = { 0 };

void model_init_system(Uint32 modelCount) {
	if (model_manager.modelCount != 0) {
		slog("Cannot init Model system, already initilized");
		return;
	}
	if (modelCount == 0) {
		slog("Cannot initalize Model system for ZERO Models"); 
		return;
	}
	model_manager.modelCount = modelCount;
	model_manager.modelList = gfc_allocate_array(sizeof(Model), modelCount);
	gf3d_mesh_init(modelCount);
}

Model* gf3d_model_new() {
	int i;
	slog("Attempt to make model %i", model_manager.modelCount);
	for (i = 0; i < model_manager.modelCount; i++) {
		slog("Model ref count %i", model_manager.modelList[i]._refCount);
		if (model_manager.modelList[i]._refCount <= 0) {
			model_manager.modelList[i]._refCount += 1;
			
			model_manager.modelCount += 1;
			return &model_manager.modelList[i];	
		}
	}

	return NULL;
}

Model* gf3d_model_load_from_file(const char* filename) {
	int i;
	SJson* json = sj_load(filename);
	SJson* mode = sj_object_get_value(json, "model");
	if (!json) {
		slog("model Cant load from file: %s", filename);
		return NULL;
	}
	if (!mode) {
		slog("model Cant find model in file: %s", filename);
		sj_free(json);
		return NULL;
	}


	Model* mod = gf3d_model_new();
	if (!mod) {
		slog("Failed to alloacate Model");
		return;
	}

	mod->mesh = gf3d_mesh_load_obj(sj_object_get_string(mode, "obj"));
	mod->Texture = gf3d_texture_load(sj_object_get_string(mode, "texture"));

	if (json) { sj_object_free(json); }

	if (!mod->mesh) { 
		slog("No mesh");
		return NULL; 
	}
	if (!mod->Texture) { 
		slog("No Texture");
		return NULL; 
	}
	return mod;

}

void gf3d_model_free(Model* model) {
	if (!model)return;
	model->_refCount--;
	/*if (mesh->_refCount > 0)return;*/
}

void gf3d_model_close() { // Remove all meshcounts
	int i;
	for (i = 0; i < model_manager.modelCount; i++) {
		gf3d_mesh_free(&model_manager.modelList[i]);
	}
	free(model_manager.modelList);
	memset(&model_manager, 0, sizeof(model_manager));
}

void gf3d_model_queue_render(Model* model, GFC_Matrix4 modelMat, GFC_Color colorMod) {
	slog("Queue Render");
	MeshUBO ubo = gf3d_mesh_get_ubo(modelMat, colorMod);
	gf3d_mesh_queue_render(model->mesh, gf3d_mesh_get_pipe(), NULL, model->Texture);
}
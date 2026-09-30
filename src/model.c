#include "simple_logger.h"

#include "gfc_types.h"


#include "model.h"


typedef struct {
	Model* modelList;
	Uint32 modelCount;
	Pipeline* pipe;
	VkDevice device;
}ModelManager;

static ModelManager model)manager = { 0 };

void model_init_system(Uint32 modelCount) {
	if (mesh_manager.meshCount != 0) {
		slog("Cannot init mesh system, already initilized");
		return;
	}
	if (mesh_max == 0) {
		slog("Cannot initalize mesh system for ZERO meshes"); 
		return;
	}
	model_manager.meshList = gfc_allocate_array(sizeof(Mesh))
}
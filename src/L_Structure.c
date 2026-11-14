#include "L_Structure.h"

Entity* Structure_load_from_file(const char* filename, int scale, int gravityScale) {
	Entity* structure = Entity_load_from_file(filename);

	/*structure->data = (structureData*){ 0 };*/
	
	//StructureData* sData = (structure->data);
	//memset(sData, 0, sizeof(StructureData));

	//sData.cameraRotation = gfc_vector3d(0, 0, 0);
	//sData.speed = 250;

	
	/*sData->gravityRadius = scale * gravityScale;*/
	structure->radius = scale;
	structure->scale = gfc_vector3d(scale, scale, scale);
	structure->Think = NULL;
	structure->Collision = NULL;

	return structure;
}
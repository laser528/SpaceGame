#include "L_Structure.h"
#include "gf3d_obj_load.h"

int checkPointCollision(Entity* self, Entity* target) {
	GFC_Sphere targetSphere = gfc_sphere(target->translation.x, target->translation.y, target->translation.z, target->radius);
	MeshPrimitive* prim;
    int c = gfc_list_get_count(self->model->mesh->primitives);
	
    for (int i = 0; i < c; i++) {
        prim = gfc_list_nth(self->model->mesh->primitives, i);
        if (!prim)continue;
		for (int j = 0; j < prim->objData->face_count; j++) {
			for (int k = 0; k < 3; i++) {
				GFC_Vector3D point = prim->objData->faceVertices[prim->objData->outFace->verts[k]].vertex;
				if (gfc_point_in_sphere(point, targetSphere)) {return 1;}
			}
		}
    }
	return 0;
	/*for (int i=0; )*/
}

Entity* Structure_load_from_file(const char* filename, int scale, int gravityScale) {
	Entity* structure = Entity_load_from_file(filename);

	structure->data = gfc_allocate_array(sizeof(StructureData), 1);
	

	StructureData* sData = (structure->data);

	//sData.cameraRotation = gfc_vector3d(0, 0, 0);
	//sData.speed = 250;

	
	sData->gravityRadius = scale * gravityScale;
	structure->radius = scale;
	structure->scale = gfc_vector3d(scale, scale, scale);
	structure->Think = NULL;
	structure->Collision = checkPointCollision;

	return structure;
}


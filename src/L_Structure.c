#include "L_Structure.h"
#include "gf3d_obj_load.h"



int checkSphereCollision(Entity* self, Entity* target) {
	GFC_Sphere targetSphere = gfc_sphere(target->translation.x, target->translation.y, target->translation.z, target->radius);
	GFC_Sphere selfSphere = gfc_sphere(self->translation.x, self->translation.y, self->translation.z, self->radius);

	return gfc_sphere_overlap(selfSphere, targetSphere);
}

int checkEdgeToMesh(Entity* self, Entity* target) {
	MeshPrimitive* prim;
	GFC_Vector3D collisionPoint;
	GFC_Sphere targetSphere;
	GFC_Vector3D direction;

	targetSphere = gfc_sphere(target->translation.x, target->translation.y, target->translation.z, target->radius);
	direction = gfc_vector3d_subbed(self->translation, target->translation);

	gfc_vector3d_normalize(&direction);

	GFC_Edge3D edge = gfc_edge3d_from_vectors(
		target->translation,
		gfc_vector3d_added(gfc_vector3d_scaled(direction, target->radius), target->translation)
	);

	int c = gfc_list_get_count(self->model->mesh->primitives);

	for (int i = 0; i < c; i++) {
		prim = gfc_list_nth(self->model->mesh->primitives, i);
		if (!prim)continue;
		for (int j = 0; j < prim->objData->face_count; j++) {


			GFC_Triangle3D tri = gfc_triangle(
				gfc_vector3d_added(gfc_vector3d_scaled_by_3D(prim->objData->faceVertices[prim->objData->outFace[j].verts[0]].vertex, self->scale), self->translation),
				gfc_vector3d_added(gfc_vector3d_scaled_by_3D(prim->objData->faceVertices[prim->objData->outFace[j].verts[1]].vertex, self->scale), self->translation),
				gfc_vector3d_added(gfc_vector3d_scaled_by_3D(prim->objData->faceVertices[prim->objData->outFace[j].verts[2]].vertex, self->scale), self->translation)
			);

			if (gfc_triangle_edge_test(edge, tri, &collisionPoint)) {
				return 1;
			}
				
		}
	}
	return 0;
	/*for (int i=0; )*/
}

int testGravity(Entity* self, Entity* target) {

}

Entity* Structure_load_from_file(const char* filename, float scale, float gravityScale) {
	Entity* structure = Entity_load_from_file(filename);

	structure->data = gfc_allocate_array(sizeof(StructureData), 1);
	

	StructureData* sData = (structure->data);

	//sData.cameraRotation = gfc_vector3d(0, 0, 0);
	//sData.speed = 250;

	
	sData->gravityRadius = scale * gravityScale;
	structure->radius = scale;
	structure->scale = gfc_vector3d(scale, scale, scale);
	structure->Think = NULL;
	structure->Collision = checkEdgeToMesh;

	return structure;
}


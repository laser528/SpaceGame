#ifndef __MODEL_H__
#define __MODEL_H__

#include "gf3d_mesh.h"
#include "gf3d_texture.h"

typedef struct
{
    GFC_Matrix4     model;
    GFC_Matrix4     view;
    GFC_Matrix4     proj;
    GFC_Vector4D    color;
 /*   GFC_Vector4D    camera;*/
}ModelUBO;

typedef struct {
	int				 _refCount;
	Texture*		 Texture;
	Mesh*			 mesh;    

}Model;

void model_init_system(Uint32 modelCount);

Model* gf3d_model_new();

Model* gf3d_model_load_from_file(const char* filename);

void gf3d_model_free(Model* model);

void gf3d_model_close();

void gf3d_model_queue_render(Model* model, GFC_Matrix4 modelMat, GFC_Color colorMod);


#endif 

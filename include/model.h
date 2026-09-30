#ifndef __MODEL_H__
#define __MODEL_H__

#include "gf3d_mesh.h"

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

#endif 

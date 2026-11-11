#pragma once
#include "simple_logger.h"
#include "gfc_primitives.h"

#include "model.h"


//typedef struct Entity Entity;

typedef struct Entity
{
    Model*          model;
    short int       id;
    int				_refCount;

    // properties
    GFC_Color       colorMod;

    //3d Space      gfc_matrix4_from_vectors_q
    GFC_Vector3D    translation; // Position
    GFC_Vector4D    quaternion;  // Rotation
    GFC_Vector3D    scale;
    GFC_Matrix4     view;
   

    int (*Think)(struct Entity* self);
    int (*Update)(struct Entity* self);

    void*           data;
  /*  int (*Class)(Entity* self);*/

}Entity;

void Entity_init_system(Uint32 modelCount);

void Entity_Think();

Entity* Entity_new();

Entity* Entity_load_from_file(const char* filename);

void Entity_free(Entity* self);

void Entity_close();

void Entity_Update();

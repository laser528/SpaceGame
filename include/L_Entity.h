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

    // Collision
    float             radius; // to create bounding box in gfc primatives

    //3d Space      gfc_matrix4_from_vectors_q
    GFC_Vector3D    translation; // Position
    GFC_Vector4D    quaternion;  // Rotation
    GFC_Vector3D    scale;
    GFC_Matrix4     view;

    GFC_Vector3D*    gravityPoint;
   

    int (*Think)(struct Entity* self);
    int (*Update)(struct Entity* self);
    int (*Collision)(struct Entity* self, struct Entity* target);
    

    void*           data;
  /*  int (*Class)(Entity* self);*/

}Entity;

void Entity_init_system(Uint32 modelCount);

void Entity_Think();

void general_update(Entity* self);

int check_collision(Entity* self);

Entity* Entity_new();

Entity* Entity_load_from_file(const char* filename);

void Entity_free(Entity* self);

void Entity_close();

void Entity_Update();

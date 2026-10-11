#ifndef __GF2D_SPRITE_H__
#define __GF2D_SPRITE_H__

#include <string.h>

#include "simple_logger.h"
#include "simple_json.h"

#include "gfc_types.h"
#include "gfc_vector.h"
#include "gfc_matrix.h"
#include "gfc_text.h"


#include "gf3d_buffers.h"
#include "gf3d_vgraphics.h"
#include "gf3d_pipeline.h"
#include "gf3d_texture.h"
#include "gf3d_obj_load.h"
#include "gf3d_mesh.h"


#define MESH_ATTRIBUTE_COUNT 3

extern int __DEBUG;

typedef struct {
    VkDevice device;
    Uint32   meshCount;
    Mesh*    meshList;
    Pipeline* pipe;
    VkVertexInputAttributeDescription   attributeDescriptions[MESH_ATTRIBUTE_COUNT];
    VkVertexInputBindingDescription     bindingDescription;
    float           drawOrder;
}MeshManager;

static MeshManager mesh_manager = { 0 };



void gf3d_mesh_close();
void gf3d_mesh_primitive_free(MeshPrimitive* prim);
Mesh* gf3d_mesh_get_by_filename(const char* filename);

void gf3d_mesh_init(Uint32 mesh_max) {
    if (mesh_manager.meshCount != 0) {
        slog("Cannot init mesh system, already init");
        return;
    }
    if (mesh_max == 0) {
        slog("cannot init mesh system for zero meshes");
        return;

    }
    mesh_manager.device = gf3d_vgraphics_get_default_logical_device();
    mesh_manager.meshList = (Mesh *)gfc_allocate_array(sizeof(Mesh), mesh_max);
    if (!mesh_manager.meshList)return;
    mesh_manager.meshCount = mesh_max;

    gf3d_mesh_get_attribute_descriptions();
    mesh_manager.pipe = gf3d_pipeline_create_from_config(
        gf3d_vgraphics_get_default_logical_device(),
        "config/model_pipeline.cfg",
        gf3d_vgraphics_get_view_extent(),
        mesh_max,
        gf3d_mesh_get_bind_description(),
        gf3d_mesh_get_attribute_descriptions(),
        MESH_ATTRIBUTE_COUNT,
        sizeof(MeshUBO),
        VK_INDEX_TYPE_UINT16
    );

    if (__DEBUG)slog("Mesh manager initiliazed");

    atexit(gf3d_mesh_close);
}

Mesh* gf3d_mesh_new() {
    int i;
    for (i = 0; i < mesh_manager.meshCount; i++) {
        if ((mesh_manager.meshList[i]._refCount <= 0) && (strlen(mesh_manager.meshList[i].filename) == 0)) {
            mesh_manager.meshList[i]._refCount += 1;

            if (mesh_manager.meshList[i].primitives) gfc_list_delete(mesh_manager.meshList[i].primitives);

            mesh_manager.meshList[i].primitives = gfc_list_new();
            if (mesh_manager.meshList[i].primitives == NULL) {
                mesh_manager.meshList[i]._refCount -= 1;
                slog("cannot allocated more mem for new mesh");
                return NULL;
            }
            mesh_manager.meshCount += 1;
            return &mesh_manager.meshList[i];
        }
    }

    return NULL;
    //GFC_TextLine filename;
    //Unit32 _refCount;
    //GFC_List* primitives;
    //GFC_box bounds;

}


Mesh* gf3d_mesh_load_obj(const char* filename) {
    Mesh* mesh;
    MeshPrimitive* prim;
    ObjData* obj;

    if (!filename) { 
        slog("Mesh load obj failed no file name");
        return NULL; 
    }

    mesh = gf3d_mesh_get_by_filename(filename);
    if (mesh) return mesh;
    slog("Failed to find mesh by filename %s", filename);

    mesh = gf3d_mesh_new();

    if (mesh) { 
        slog("mesh allocated");
        strcpy(mesh->filename, filename);
        prim = gf3d_mesh_primitive_new();
        if (prim) {
            slog("prim allocated");
            obj = gf3d_obj_load_from_file(filename);
            if (obj) {
                slog("obj allocated");
                prim->objData = obj;
                gfc_list_append(mesh->primitives, prim);
                return mesh;
            }

            gf3d_mesh_free(mesh);
            gf3d_mesh_primitive_free(prim);
            slog("failed to load mesh primatives %s", filename);
        }
        slog("failed to alloacte mesh primative %s", filename);
        
        gf3d_mesh_free(mesh);
        return NULL;
    };
    slog("Failed to load new mesh %s", filename);
    return NULL;
    
}


MeshPrimitive* gf3d_mesh_primitive_new() {
    MeshPrimitive* prim;
    prim = (MeshPrimitive*)gfc_allocate_array(sizeof(MeshPrimitive), 1); // allocates and sets mem to zero
    if (prim) {return prim;}
    slog("mesh primative failed to allocate");
    return NULL;
    
}

void gf3d_mesh_free(Mesh* mesh) {
    if (!mesh)return;
    mesh->_refCount--;
    /*if (mesh->_refCount > 0)return;*/

}

void gf3d_mesh_reset_pipes() {
    Uint32 bufferFrame = gf3d_vgraphics_get_current_buffer_frame();

    gf3d_pipeline_reset_frame(mesh_manager.pipe, bufferFrame);
    mesh_manager.drawOrder = 0;

}

void gf3d_mesh_queue_render(Mesh* mesh, Pipeline* pipe, void* uboData, Texture* texture) {

    int i, c;
    MeshPrimitive* prim;
    if (!mesh) {
        slog("cannot render a NULL mesh in render");
        return;
    }
    if (!texture) {
        slog("cannot render a NULL texture in render");
        return;
    }
    if (!uboData) {
        slog("cannot render a NULL UBO in render");
        return;
    }
 /*   if (!pipe) {
        slog("cannot render a NULL pipe in render");
        return;
    }*/

    c = gfc_list_get_count(mesh->primitives);


    for (int i = 0; i < c; i++) {
        prim = gfc_list_nth(mesh->primitives, i);
        if (!prim)continue;

        gf3d_pipeline_queue_render(
            mesh_manager.pipe,
            prim->vertexBuffer,
            prim->vertexCount,
            prim->faceBuffer,
            uboData,
            texture);

        //gf3d_pipeline_queue_render(
        //    pipe,
        //    sprite->buffer,
        //    6,//its a single quad
        //    gf2d_sprite.faceBuffer,
        //    uboData,
        //    texture);

    }
    


}

Mesh* gf3d_mesh_get_by_filename(const char* filename) {
    if (!filename)return NULL;
    int i,c;
    c = gfc_list_count(mesh_manager.meshList);

    for (i = 0; i < c; i++) {
        if (strlen(mesh_manager.meshList[i].filename) == 0)continue;
        if (gfc_strlcmp(mesh_manager.meshList[i].filename, filename) == 0) {
            return &mesh_manager.meshList[i];
        }
    }
    return NULL;
}

int* gf3d_mesh_obj_buffer_create(Mesh* mesh) { // Ima be real idk what the heck this is
    int i, c;
    MeshPrimitive* prim;

    if (!mesh)return NULL;
    prim = gf3d_mesh_primitive_new();
    if (!prim) {
        slog("Failed to get new primitive for mesh");
        return 0;
    }
    if (gf3d_mesh_primitive_buffer_create(prim)) {
        gfc_list_append(mesh->primitives, prim);
    }
    c = gfc_list_count(mesh->primitives);
    for (i = 0; i < c; i++) {
        prim = gfc_list_nth(mesh->primitives, i);
        if (!prim)continue;
        if (!gf3d_mesh_primitive_buffer_create(prim)) return 0;
    }
}

int gf3d_mesh_primitive_buffer_create(MeshPrimitive* mesh) {
    void* data;
    Uint32 count;
    Face faces[2];
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    //check if mesh and mesh objData exist 
    // get the buffer size by Uint32 sizeof(Face)*mesh->objData->face_count; 
    Uint32 bufferSize = sizeof(Face)*mesh->objData->face_count;
    //face buffers
    gf3d_buffer_create(
        bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT 
        | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory
    );
    
    vkMapMemory(mesh_manager.device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, mesh->objData->outFace, (size_t)bufferSize); // could be outFace could be FaceVerts idk
    vkUnmapMemory(mesh_manager.device, stagingBufferMemory);

    gf3d_buffer_create(
        bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        &mesh->faceBuffer, &mesh->faceBufferMemory
    );

    // Maybe dont need but dosent make much sense to me
    gf3d_buffer_copy(stagingBuffer, mesh->faceBuffer, bufferSize);

    vkDestroyBuffer(mesh_manager.device, stagingBufferMemory, NULL);
    vkFreeMemory(mesh_manager.device, stagingBufferMemory, NULL);

    //Vertex
    bufferSize = sizeof(Face) * mesh->objData->face_vert_count;

    gf3d_buffer_create(
        bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
        | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory
    );

    vkMapMemory(mesh_manager.device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, mesh->objData->face_vert_count, (size_t)bufferSize);
    vkUnmapMemory(mesh_manager.device, stagingBufferMemory);

    gf3d_buffer_create(
        bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        &mesh->vertexBuffer, &mesh->vertexBufferMemory
    );

    gf3d_buffer_copy(stagingBuffer, mesh->vertexBuffer, bufferSize);

    vkDestroyBuffer(mesh_manager.device, stagingBufferMemory, NULL);
    vkFreeMemory(mesh_manager.device, stagingBufferMemory, NULL);


    return 1;
    //vkMapMemory()

}



void gf3d_mesh_close() { // Remove all meshcounts
    int i;
    for (i = 0; i < mesh_manager.meshCount; i++) {
        gf3d_mesh_free(&mesh_manager.meshList[i]);
    }
    free(mesh_manager.meshList);
    memset(&mesh_manager, 0, sizeof(MeshManager));
}

void gf3d_mesh_primitive_free(MeshPrimitive* prim) {

    if (!prim)return;


    if (prim->objData) {
        gf3d_obj_free(prim->objData);
    }

    if (prim->faceBuffer != VK_NULL_HANDLE)
    {
        vkDestroyBuffer(mesh_manager.device, prim->faceBuffer, NULL);
    }
    if (prim->vertexBuffer != VK_NULL_HANDLE)
    {
        vkFreeMemory(mesh_manager.device, prim->vertexBuffer, NULL);
    }

   /* gf3d_texture_free(sprite->texture);*/
    memset(prim, 0, sizeof(prim));
   
}

/**
 * @brief make an exact, but separate copy of the input mesh
 * @param in the mesh to duplicate
 * @return NULL on error, or a copy of in
 */
Mesh* gf3d_mesh_copy(Mesh* in);

/**
 * @brief move all of the vertices of the mesh by offset at the buffer level
 * @param in the mesh to move
 * @param offset how much to move it
 * @param rotation apply this rotation to the vertices and normals
 */
void gf3d_mesh_move_vertices(Mesh* in, GFC_Vector3D offset, GFC_Vector3D rotation);



VkVertexInputAttributeDescription* gf3d_mesh_get_attribute_descriptions(/*Uint32* count*/) {
    mesh_manager.attributeDescriptions[0].binding = 0;
    mesh_manager.attributeDescriptions[0].location = 0;
    mesh_manager.attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    mesh_manager.attributeDescriptions[0].offset = offsetof(Vertex, vertex);

    mesh_manager.attributeDescriptions[1].binding = 0;
    mesh_manager.attributeDescriptions[1].location = 1;
    mesh_manager.attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    mesh_manager.attributeDescriptions[1].offset = offsetof(Vertex, normal);

    mesh_manager.attributeDescriptions[2].binding = 0;
    mesh_manager.attributeDescriptions[2].location = 1;
    mesh_manager.attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
    mesh_manager.attributeDescriptions[2].offset = offsetof(Vertex, texel);

    //mesh_manager.bindingDescription.binding = 0;
    //mesh_manager.bindingDescription.stride = sizeof(Vertex);
    //mesh_manager.bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    /*if (count)*count = MESH_ATTRIBUTE_COUNT;*/
    return mesh_manager.attributeDescriptions;
}

VkVertexInputBindingDescription* gf3d_mesh_get_bind_description() {
    mesh_manager.bindingDescription.binding = 0;
    mesh_manager.bindingDescription.stride = sizeof(Vertex);
    mesh_manager.bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return &mesh_manager.bindingDescription;
}





/**
 * @brief called to submit all draw commands to the mesh pipelines
 */
void gf3d_mesh_submit_pipe_commands();

/**
 * @brief get the current command buffer for the mesh system
 */
VkCommandBuffer gf3d_mesh_get_model_command_buffer();

/**
 * @brief adds a mesh to the render pass rendered as an outline highlight
 * @note: must be called within the render pass
 * @param mesh the mesh to render
 * @param com the command pool to use to handle the request we are rendering with
 */
void gf3d_mesh_render(Mesh* mesh, VkCommandBuffer commandBuffer, VkDescriptorSet* descriptorSet);

/**
 * @brief render a mesh through a given pipeline
 */
void gf3d_mesh_render_generic(Mesh* mesh, Pipeline* pipe, VkDescriptorSet* descriptorSet);

/**
 * @brief create a mesh's internal buffers based on vertices
 * @param primitive the mesh primitive to populate
 * @note the primitive must have the objData set and it must have be organizes in buffer order
 */
void gf3d_mesh_create_vertex_buffer_from_vertices(MeshPrimitive* primitive);

/**
 * @brief get the pipeline that is used to render basic 3d meshes
 * @return NULL on error or the pipeline in question
 */
Pipeline* gf3d_mesh_get_pipeline();


MeshUBO gf3d_mesh_get_ubo(GFC_Matrix4 modelMat, GFC_Color colorMod) { // Laser Ask about mesh get ubo
   /* MeshUBO* ubo = (MeshUBO*)gfc_allocate_array(sizeof(MeshUBO), 1);*/
    MeshUBO ubo = { 0 };
    gf3d_vgraphics_get_projection_matrix(ubo.proj);
    gf3d_vgraphics_get_view(ubo.view);
    memcpy(ubo.model, modelMat, sizeof(GFC_Matrix4));
    ubo.color = gfc_color_to_vector4f(colorMod);
    return ubo;
}
Pipeline* gf3d_mesh_get_pipe() {
    return mesh_manager.pipe;
}

#endif

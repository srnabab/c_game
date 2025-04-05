#include "vk_code_h/vk_load_model.h"

#define TINYOBJ_LOADER_C_IMPLEMENTATION

#define TINYOBJ_MALLOC SDL_malloc
#define TINYOBJ_REALLOC SDL_realloc
#define TINYOBJ_CALLOC SDL_calloc
#define TINYOBJ_FREE SDL_free

#include "tinyobj_loader/tinyobj_loader_c.h"

// #define CGLTF_IMPLEMENTATION
// #include "cgltf/cgltf.h"

#include "SDL3/SDL_iostream.h"

#include "G_log.h"
#include "G_resource.h"
#include "G_struct.h"

extern G_SYNC allSync;

static void tinyobj_SDL_readFile(void *ctx, const char *filename, int is_mtl, const char *obj_filename, char **buf, size_t *len)
{
    (void)ctx;

    if (filename == NULL)
    {
        *buf = NULL;
        *len = 0;
        return;
    }

    SDL_IOStream * stream;
    Sint64 size;
    char * buffer;

    stream = SDL_IOFromFile(filename, "rb");
    if (stream == NULL)
    {
        *buf = NULL;
        *len = 0;
        return;
    }
    
    SDL_SeekIO(stream, 0, SDL_IO_SEEK_END);
    size = SDL_TellIO(stream);

    buffer = (char*)SDL_malloc(size * sizeof(char));
    if (buffer == NULL)
    {   
        *buf = NULL;
        *len = 0;
        return;
    }

    SDL_SeekIO(stream, 0, SDL_IO_SEEK_SET);
    SDL_ReadIO(stream, buffer, size);

    *buf = buffer;
    *len = size;

    SDL_CloseIO(stream);
}
bool loadModelSetVertex(PathType modelPath, PathType texturePath, Vertex4 * vertices, Uint32 * pVertexIndex, Uint32 * indices, Uint32 * pIndexIndex, VkFormat textureFormat, VkImageAspectFlags flags\
, const char * innerName, VkDescriptorSet * pDescriptorSet, bool ground)
{
    tinyobj_attrib_t attrib;
    tinyobj_shape_t * shapes;
    size_t num_shapes;
    tinyobj_material_t * materials;
    size_t num_materials;
    G_Texture_P * tempTexture;

    SDL_LockMutex(allSync.textureMutex);
    Uint32 vertexIndex = *pVertexIndex;
    Uint32 indexIndex = *pIndexIndex;
    SDL_UnlockMutex(allSync.textureMutex);

    bool textureRes = loadTexture(texturePath, textureFormat, flags, innerName, pDescriptorSet);
    if (textureRes == false) return false;

    tempTexture = getTexture(innerName);
    SDL_LockMutex(allSync.textureMutex);

    tempTexture->offsets[tempTexture->refCount].offset = indexIndex;

    SDL_UnlockMutex(allSync.textureMutex);
 
    int res = tinyobj_parse_obj(&attrib, &shapes, &num_shapes, &materials, &num_materials, getPath(modelPath), tinyobj_SDL_readFile, NULL, TINYOBJ_FLAG_TRIANGULATE);

    if (res == TINYOBJ_SUCCESS) 
    {
        int i;
        if (attrib.num_vertices != attrib.num_texcoords)
        {
            for (i = 0;i < attrib.num_faces;i++)
            {
                indices[indexIndex] = i;
                vertices[vertexIndex].pos[0] = attrib.vertices[attrib.faces[i].v_idx * 3 + 0];
                vertices[vertexIndex].pos[1] = attrib.vertices[attrib.faces[i].v_idx * 3 + 1];
                vertices[vertexIndex].pos[2] = attrib.vertices[attrib.faces[i].v_idx * 3 + 2];

                vertices[vertexIndex].texCoord[0] = attrib.texcoords[attrib.faces[i].vt_idx * 2 + 0];
                vertices[vertexIndex].texCoord[1] = 1 - attrib.texcoords[attrib.faces[i].vt_idx * 2 + 1];

                vertices[vertexIndex].color[0] = 1.0f;
                vertices[vertexIndex].color[1] = 1.0f;
                vertices[vertexIndex].color[2] = 1.0f;

                vertices[vertexIndex].normal[0] = attrib.normals[attrib.faces[i].vn_idx * 3 + 0];
                vertices[vertexIndex].normal[1] = attrib.normals[attrib.faces[i].vn_idx * 3 + 1];
                vertices[vertexIndex].normal[2] = attrib.normals[attrib.faces[i].vn_idx * 3 + 2];

                if (ground)
                {
                    float dot = glm_vec3_dot(vertices[vertexIndex].normal, (vec3){0.0f, 0.0f, 1.0f});
                    if (SDL_abs(dot - 0.0f) < 0.0001f)
                    {
                        vertices[vertexIndex].groupId = 1;
                    }
                    else vertices[vertexIndex].groupId = 0;
                }
                else vertices[vertexIndex].groupId = 0;

                indexIndex++;
                vertexIndex++;
            }
            *pIndexIndex = indexIndex;
            *pVertexIndex = vertexIndex;
        }
        else
        {
            int i;
            Uint32 * v_vt_index = (Uint32*)SDL_malloc(attrib.num_vertices * sizeof(Uint32));
            Uint32 * v_vn_index = (Uint32*)SDL_malloc(attrib.num_vertices * sizeof(Uint32));
            for (i = 0;i < attrib.num_faces;i++)
            {
                indices[indexIndex] = attrib.faces[i].v_idx;

                v_vt_index[attrib.faces[i].v_idx] = attrib.faces[i].vt_idx;
                // print("v vt: %d, %d", attrib.faces[i].v_idx, attrib.faces[i].vt_idx);
                v_vn_index[attrib.faces[i].v_idx] = attrib.faces[i].vn_idx;

                indexIndex++;
            }
            *pIndexIndex = indexIndex;

            for (i = 0;i < attrib.num_vertices;i++)
            {
                vertices[vertexIndex].pos[0] = attrib.vertices[vertexIndex * 3 + 0];
                vertices[vertexIndex].pos[1] = attrib.vertices[vertexIndex * 3 + 1];
                vertices[vertexIndex].pos[2] = attrib.vertices[vertexIndex * 3 + 2];

                vertices[vertexIndex].texCoord[0] = attrib.texcoords[v_vt_index[i] * 2 + 0];
                vertices[vertexIndex].texCoord[1] = 1.0f - attrib.texcoords[v_vt_index[i] * 2 + 1];
                // print("texCoord(%d): %f, %f", i, vertices[vertexIndex].texCoord[0], vertices[vertexIndex].texCoord[1]);
                // vertices[vertexIndex].texCoord[0] = sdl_randf();
                // vertices[vertexIndex].texCoord[1] = sdl_randf();

                vertices[vertexIndex].color[0] = 1.0f;
                vertices[vertexIndex].color[1] = 1.0f;
                vertices[vertexIndex].color[2] = 1.0f;

                vertices[vertexIndex].normal[0] = attrib.normals[v_vn_index[i] * 3 + 0];
                vertices[vertexIndex].normal[1] = attrib.normals[v_vn_index[i] * 3 + 1];
                vertices[vertexIndex].normal[2] = attrib.normals[v_vn_index[i] * 3 + 2];

                vertexIndex++;
            }
            *pVertexIndex = vertexIndex;

            SDL_free(v_vt_index);
            SDL_free(v_vn_index);
        }
        print("load model success");
    }
    else 
    {
        unloadTexture(innerName);
        print("tiny obj load fail: %d", res);
        return false;
    }

    SDL_LockMutex(allSync.textureMutex);

    tempTexture->offsets[tempTexture->refCount].count = attrib.num_faces;
    tempTexture->refCount++;

    SDL_UnlockMutex(allSync.textureMutex);

    tinyobj_attrib_free(&attrib);
    tinyobj_shapes_free(shapes, num_shapes);
    tinyobj_materials_free(materials, num_materials);

    return true;
}
// static void * cgltf_SDL_alloc(void * user_data, size_t size)
// {
//     (void)user_data;
//     return SDL_malloc(size);
// }
// static void cgltf_SDL_free(void * user_data, void * ptr)
// {
//     (void)user_data;
//     SDL_free(ptr);
// }
// static cgltf_result cgltf_SDL_file_read(const struct cgltf_memory_options* memory_options, const struct cgltf_file_options* file_options, const char* path, cgltf_size* size, void** data)
// {
// 	(void)file_options;
// 	void* (*memory_alloc)(void*, cgltf_size) = memory_options->alloc_func ? memory_options->alloc_func : &cgltf_default_alloc;
// 	void (*memory_free)(void*, void*) = memory_options->free_func ? memory_options->free_func : &cgltf_default_free;

// 	SDL_IOStream * file = SDL_IOFromFile(path, "rb");
// 	if (!file)
// 	{
// 		return cgltf_result_file_not_found;
// 	}

// 	cgltf_size file_size = size ? *size : 0;

// 	if (file_size == 0)
// 	{
// 		SDL_SeekIO(file, 0, SDL_IO_SEEK_END);

// 		long length = SDL_TellIO(file);

// 		if (length < 0)
// 		{
// 			SDL_CloseIO(file);
// 			return cgltf_result_io_error;
// 		}

// 		SDL_SeekIO(file, 0, SDL_IO_SEEK_SET);
// 		file_size = (cgltf_size)length;
// 	}

// 	char* file_data = (char*)memory_alloc(memory_options->user_data, file_size);
// 	if (!file_data)
// 	{
// 		SDL_CloseIO(file);
// 		return cgltf_result_out_of_memory;
// 	}

// 	cgltf_size read_size = SDL_ReadIO(file, file_data, file_size);

// 	SDL_CloseIO(file);

// 	if (read_size != file_size)
// 	{
// 		memory_free(memory_options->user_data, file_data);
// 		return cgltf_result_io_error;
// 	}

// 	if (size)
// 	{
// 		*size = file_size;
// 	}
// 	if (data)
// 	{
// 		*data = file_data;
// 	}

// 	return cgltf_result_success;
// }
// bool loadModel(PathType modelPath)
// {
//     cgltf_options options = {};
//     options.type = cgltf_file_type_invalid;
//     options.json_token_count = 0;

//     cgltf_memory_options memory_options = {};
//     memory_options.user_data = NULL;
//     memory_options.alloc_func = cgltf_SDL_alloc;
//     memory_options.free_func = cgltf_SDL_free;

//     options.memory = memory_options;

//     cgltf_file_options file_options = {};
//     file_options.user_data = NULL;
//     file_options.read = cgltf_SDL_file_read;
//     file_options.release = NULL;

//     options.file = file_options;
//     cgltf_data * data = NULL;

//     cgltf_result res = cgltf_parse_file(&options, getPath(modelPath), &data);

//     if (res != cgltf_result_success)
//     {
//         print("cgltf parse file fail: %d", res);
//         return false;
//     }

//     res = cgltf_load_buffers(&options, data, getPath(modelPath));

//     if (res != cgltf_result_success)
//     {
//         print("cgltf load buffers fail: %d", res);
//         cgltf_free(data);
//         return false;
//     }

    Uint32 i, j;
    // cgltf_primitive * primitive = data->mesh->primitives;
    // for (i = 0;i < primitive->attributes_count;i++)
    // {
    //     print("name: %s", primitive->attributes[i].name);
    //     if (primitive->attributes[i].data != NULL)
    //     {
    //         for (j = 0;j < data->accessors[i].count;j++)
    //         {
    //             if (data->accessors[i].component_type == cgltf_component_type_r_32f)
    //             {
    //                 print("value: %f", *(float*)(data->accessors[i].buffer_view->buffer->data + j * data->accessors[i].stride));
    //             }
    //             else if (data->accessors[i].component_type == cgltf_component_type_r_16u)
    //             {
    //                 print("value: %u", *(Uint16*)(data->accessors[i].buffer_view->buffer->data + j * data->accessors[i].stride));
    //             }
    //             else if (data->accessors[i].component_type == cgltf_component_type_r_8u)
    //             {
    //                 print("value: %u", *(Uint8*)(data->accessors[i].buffer_view->buffer->data + j * data->accessors[i].stride));
    //             }
    //         }
    //     }
    // }

//     cgltf_free(data);

//     return true;
// }
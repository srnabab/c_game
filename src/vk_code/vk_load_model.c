#include "vk_load_model.h"

#define TINYOBJ_LOADER_C_IMPLEMENTATION

#define TINYOBJ_MALLOC SDL_malloc
#define TINYOBJ_REALLOC SDL_realloc
#define TINYOBJ_CALLOC SDL_calloc
#define TINYOBJ_FREE SDL_free

#include "tinyobj_loader/tinyobj_loader_c.h"

#include "G_file/G_file.h"
#include "SDL3/SDL_iostream.h"
#include "G_log.h"

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
}
void loadModel(const char * filePath, Vertex * vertices, Uint32 * pVertexIndex, Uint32 * indices, Uint32 * pIndexIndex)
{
    tinyobj_attrib_t attrib;
    tinyobj_shape_t * shapes;
    size_t num_shapes;
    tinyobj_material_t * materials;
    size_t num_materials;

    int res = tinyobj_parse_obj(&attrib, &shapes, &num_shapes, &materials, &num_materials, filePath, tinyobj_SDL_readFile, NULL, TINYOBJ_FLAG_TRIANGULATE);

    if (res == TINYOBJ_SUCCESS) 
    {
        print("Load model success");
        Uint32 vertexIndex = *pVertexIndex;
        Uint32 indexIndex = *pIndexIndex;
        int i;
        for (i = 0;i < attrib.num_vertices;i++)
        {
            vertices[vertexIndex].pos[0] = attrib.vertices[i * 3 + 0];
            vertices[vertexIndex].pos[1] = attrib.vertices[i * 3 + 1];
            vertices[vertexIndex].pos[2] = attrib.vertices[i * 3 + 2];

            vertices[vertexIndex].texCoord[0] = attrib.texcoords[i * 2 + 0];
            vertices[vertexIndex].texCoord[1] = attrib.texcoords[i * 2 + 1];

            vertices[vertexIndex].color[0] = 1.0f;
            vertices[vertexIndex].color[1] = 1.0f;
            vertices[vertexIndex].color[2] = 1.0f;

            vertexIndex++;
        }
        *pVertexIndex = vertexIndex;
        for (i = 0;i < attrib.num_faces;i++)
        {
            indices[indexIndex] = attrib.faces[i].v_idx - 1;

            indexIndex++;
        }
        *pIndexIndex = indexIndex;
    }
    else print("tiny obj load fail: %d", res);

    tinyobj_attrib_free(&attrib);
    tinyobj_shapes_free(shapes, num_shapes);
    tinyobj_materials_free(materials, num_materials);
}
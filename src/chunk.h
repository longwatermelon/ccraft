#ifndef CHUNK_H
#define CHUNK_H

#include "render.h"
#include "texture.h"
#include <sys/types.h>
#include <cglm/cglm.h>

struct World;

enum
{
    SIDE_TOP,
    SIDE_SIDE,
    SIDE_BOT
};

enum
{
    BLOCK_AIR,
    BLOCK_GRASS,
    BLOCK_DIRT
};

struct CubeTexture
{
    vec2 top, bottom, side;
};

struct CubeTexture *ct_alloc(vec2 top, vec2 bottom, vec2 side);
void ct_free(struct CubeTexture *ct);

struct Chunk
{
    vec3 pos;
    int grid[16][256][16];
    int heightmap[16][16];

    struct World *world;
};

struct Chunk *chunk_alloc(struct World *w, vec3 pos);
void chunk_free(struct Chunk *c);

/* void chunk_render(struct Chunk *c, RenderInfo *ri, struct CubeTexture *tex); */
/* void chunk_render_cube(struct Chunk *c, RenderInfo *ri, int x, int y, int z, struct CubeTexture *tex); */
/* void chunk_render_face(struct Chunk *c, RenderInfo *ri, int x, int y, int z, float *face, struct Texture *tex); */

// Return number of visible verts
// Modify vertbuffer, n
size_t chunk_visible_verts(struct Chunk *c, int side, struct Camera *cam, float **vertbuffer, size_t *n);
void chunk_face_at(struct Chunk *c, ivec3 pos, float **verts, size_t *nverts, size_t *capacity, float *face);

int chunk_get(struct Chunk *c, ivec3 pos);

void chunk_find_highest(struct Chunk *c);

#endif


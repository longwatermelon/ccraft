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

enum
{
    BSTATE_OPEN,
    BSTATE_ENCLOSED
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

    int block_states[16][256][16];

    struct World *world;
};

struct Chunk *chunk_alloc(struct World *w, vec3 pos);
void chunk_free(struct Chunk *c);

// Modify vertbuffer, n, capacity
void chunk_visible_verts(struct Chunk *c, struct Camera *cam, float **vertbuffer, size_t *count, size_t *capacity, ivec3 selected);
void chunk_face_at(struct Chunk *c, ivec3 pos, float **verts, size_t *nverts, size_t *capacity, float *face, ivec3 selected);

int chunk_get(struct Chunk *c, ivec3 pos, bool check_adjacent);

void chunk_find_highest(struct Chunk *c);
void chunk_update_blockstates(struct Chunk *c);
void chunk_gen_terrain(struct Chunk *c);

#endif


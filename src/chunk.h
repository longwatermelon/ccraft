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

struct CubeTexture
{
    struct Texture *top, *bottom, *side;
};

struct CubeTexture *ct_alloc(const char *top, const char *bot, const char *side);
void ct_free(struct CubeTexture *ct);

struct Chunk
{
    vec3 pos;
    int grid[16][256][16];

    struct World *world;
};

struct Chunk *chunk_alloc(struct World *w, vec3 pos);
void chunk_free(struct Chunk *c);

/* void chunk_render(struct Chunk *c, RenderInfo *ri, struct CubeTexture *tex); */
/* void chunk_render_cube(struct Chunk *c, RenderInfo *ri, int x, int y, int z, struct CubeTexture *tex); */
/* void chunk_render_face(struct Chunk *c, RenderInfo *ri, int x, int y, int z, float *face, struct Texture *tex); */

float *chunk_visible_verts(struct Chunk *c, int side, size_t *n);
void chunk_face_at(struct Chunk *c, ivec3 pos, float *face, float dest[48]);

int chunk_get(struct Chunk *c, ivec3 pos);

#endif


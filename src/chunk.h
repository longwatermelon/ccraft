#ifndef CHUNK_H
#define CHUNK_H

#include "render.h"
#include <cglm/cglm.h>

struct Chunk
{
    vec3 pos;
    int grid[16][256][16];
};

struct Chunk *chunk_alloc(vec3 pos);
void chunk_free(struct Chunk *c);

void chunk_render(struct Chunk *c, RenderInfo *ri);
void chunk_render_cube(struct Chunk *c, RenderInfo *ri, int x, int y, int z);
void chunk_render_face(struct Chunk *c, RenderInfo *ri, int x, int y, int z, float *face);

int chunk_get(struct Chunk *c, int x, int y, int z);

void chunk_init_renderer();

#endif


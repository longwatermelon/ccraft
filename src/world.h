#ifndef WORLD_H
#define WORLD_H

#include "chunk.h"
#include "render.h"

struct World
{
    struct Chunk **chunks;
    size_t nchunks;

    struct CubeTexture *tex;
};

struct World *world_alloc();
void world_free(struct World *w);

void world_render(struct World *w, RenderInfo *ri);
void world_render_side(struct World *w, RenderInfo *ri, struct Chunk *c, int side);

struct Chunk *world_adjacent_chunk(struct World *w, struct Chunk *c, vec3 dir);

void world_init_renderer();

#endif


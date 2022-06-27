#ifndef WORLD_H
#define WORLD_H

#include "chunk.h"
#include "render.h"

#define RENDER_DISTANCE 6

struct World
{
    struct Chunk *chunks[RENDER_DISTANCE][RENDER_DISTANCE];

    struct CubeTexture **texs;
    size_t ntexs;

    struct Texture *atlas;
};

struct World *world_alloc();
void world_free(struct World *w);

void world_render(struct World *w, RenderInfo *ri);
void world_render_side(struct World *w, RenderInfo *ri, struct Chunk *c, int side);

struct Chunk *world_adjacent_chunk(struct World *w, struct Chunk *c, vec3 dir);

struct CubeTexture *world_get_tex(struct World *w, int block);

void world_init_renderer();

#endif


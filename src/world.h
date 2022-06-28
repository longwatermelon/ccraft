#ifndef WORLD_H
#define WORLD_H

#include "chunk.h"
#include "render.h"

#define RENDER_DISTANCE 12

struct World
{
    struct Chunk *chunks[RENDER_DISTANCE][RENDER_DISTANCE];

    struct CubeTexture **texs;
    size_t ntexs;

    struct Texture *atlas;

    float *vertbuffer;
    size_t vertbuffer_size;
};

struct World *world_alloc();
void world_free(struct World *w);

void world_render(struct World *w, RenderInfo *ri);

struct Chunk *world_adjacent_chunk(struct World *w, struct Chunk *c, vec3 dir);

struct CubeTexture *world_get_tex(struct World *w, int block);

int world_get_block(struct World *w, vec3 pos, struct Chunk **chunk);

void world_init_renderer();

#endif


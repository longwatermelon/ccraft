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

// out: chunk, index
int world_get_block(struct World *w, vec3 pos, struct Chunk **chunk, ivec3 index);

// out: coords
float world_cast_ray(struct World *w, struct Camera *cam, struct Chunk **chunk, ivec3 coords);
float world_cast_rayx(struct World *w, struct Camera *cam, struct Chunk **c, ivec3 coords);
float world_cast_rayy(struct World *w, struct Camera *cam, struct Chunk **c, ivec3 coords);
float world_cast_rayz(struct World *w, struct Camera *cam, struct Chunk **c, ivec3 coords);

void world_center(struct World *w, vec3 dest);

void world_gen_chunks(struct World *w, vec3 cam);

void world_init_renderer();

#endif


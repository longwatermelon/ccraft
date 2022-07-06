#ifndef WORLD_H
#define WORLD_H

#include "chunk.h"
#include "render.h"

#define RENDER_DISTANCE 12

enum
{
    RAYI_X,
    RAYI_Y,
    RAYI_Z
};

struct World
{
    struct Chunk *chunks[RENDER_DISTANCE][RENDER_DISTANCE];

    struct CubeTexture **texs;
    size_t ntexs;

    struct Texture *atlas;

    float *vertbuffer;
    size_t vertbuffer_size;

    struct Chunk *back_left;
    size_t cz, cx; // Measured in chunks (z len, x len)
};

struct World *world_alloc();
void world_free(struct World *w);

void world_render(struct World *w, RenderInfo *ri, ivec3 selected);

struct Chunk *world_adjacent_chunk(struct World *w, struct Chunk *c, vec3 dir);

struct CubeTexture *world_get_tex(struct World *w, int block);

// out: chunk, index
int world_get_block(struct World *w, vec3 pos, struct Chunk **chunk, ivec3 index);

// out: coords
float world_cast_ray(struct World *w, struct Camera *cam, struct Chunk **chunk, ivec3 coords, int *type);
float world_cast_rayx(struct World *w, struct Camera *cam, struct Chunk **c, ivec3 coords);
float world_cast_rayy(struct World *w, struct Camera *cam, struct Chunk **c, ivec3 coords);
float world_cast_rayz(struct World *w, struct Camera *cam, struct Chunk **c, ivec3 coords);

void world_center(struct World *w, vec3 dest);

void world_gen_chunks(struct World *w, vec3 cam);
void world_gen_chunks_front(struct World *w);
void world_gen_chunks_back(struct World *w);
void world_gen_chunks_left(struct World *w);
void world_gen_chunks_right(struct World *w);

// Connects chunks together only inside of w->chunks
void world_connect_chunks(struct World *w);
// Fill w->chunks starting with back_left at [0][0].
void world_fill_chunk_array(struct World *w, struct Chunk *back_left);

void world_init_renderer();

// Start from w->back_left, move <x> along x axis and <z> along z axis
struct Chunk *world_get_chunk(struct World *w, int x, int z);

#endif


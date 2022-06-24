#include "world.h"


struct World *world_alloc()
{
    struct World *w = malloc(sizeof(struct World));
    w->chunks = malloc(sizeof(struct Chunk*));
    w->nchunks = 1;
    w->chunks[0] = chunk_alloc((vec3){ 0.f, -15.f, 0.f });

    w->tex = ct_alloc("res/cube/top.png", "res/cube/bottom.png", "res/cube/side.png");

    return w;
}


void world_free(struct World *w)
{
    ct_free(w->tex);

    for (size_t i = 0; i < w->nchunks; ++i)
        chunk_free(w->chunks[i]);

    free(w->chunks);
    free(w);
}


void world_render(struct World *w, RenderInfo *ri)
{
    for (size_t i = 0; i < w->nchunks; ++i)
        chunk_render(w->chunks[i], ri, w->tex);
}


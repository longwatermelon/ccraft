#include "world.h"
#include <glad/glad.h>

unsigned int g_vao, g_vb;
size_t g_vb_size;

struct World *world_alloc()
{
    struct World *w = malloc(sizeof(struct World));

    int i = RENDER_DISTANCE / 2 * 16;

    for (int x = -i; x < i; x += 16)
    {
        for (int z = -i; z < i; z += 16)
        {
            w->chunks[(x + i) / 16][(z + i) / 16] = chunk_alloc(w, (vec3){ x, 0.f, z });
        }
    }

    w->ntexs = 2;
    w->texs = malloc(sizeof(struct CubeTexture*) * w->ntexs);
    w->texs[0] = ct_alloc((vec2){ 0.f, 0.f }, (vec2){ 200.f, 0.f }, (vec3){ 100.f, 0.f });
    w->texs[1] = ct_alloc((vec2){ 200.f, 0.f }, (vec2){ 200.f, 0.f }, (vec2){ 200.f, 0.f });

    w->atlas = tex_alloc("res/atlas.png");

    return w;
}


void world_free(struct World *w)
{
    for (size_t i = 0; i < w->ntexs; ++i)
        ct_free(w->texs[i]);

    free(w->texs);

    tex_free(w->atlas);

    free(w);
}


void world_render(struct World *w, RenderInfo *ri)
{
    mat4 model;
    shader_mat4(ri->shader, "view", ri->view);
    shader_mat4(ri->shader, "projection", ri->proj);

    tex_bind(w->atlas, 0);

    for (int x = 0; x < RENDER_DISTANCE; ++x)
    {
        for (int z = 0; z < RENDER_DISTANCE; ++z)
        {
            struct Chunk *chunk = w->chunks[x][z];

            glm_mat4_identity(model);
            glm_translate(model, chunk->pos);
            shader_mat4(ri->shader, "model", model);

            world_render_side(w, ri, chunk, SIDE_TOP);
            world_render_side(w, ri, chunk, SIDE_SIDE);
            world_render_side(w, ri, chunk, SIDE_BOT);
        }
    }
}


void world_render_side(struct World *w, RenderInfo *ri, struct Chunk *c, int side)
{
    size_t n;
    float *verts = chunk_visible_verts(c, side, ri->cam, &n);

    if (n)
    {
        glBindBuffer(GL_ARRAY_BUFFER, g_vb);

        if (n * sizeof(float) > g_vb_size)
        {
            glBufferData(GL_ARRAY_BUFFER, n * sizeof(float), verts, GL_DYNAMIC_DRAW);
            g_vb_size = n * sizeof(float);
        }
        else
        {
            glBufferSubData(GL_ARRAY_BUFFER, 0, n * sizeof(float), verts);
        }

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindVertexArray(g_vao);
        glDrawArrays(GL_TRIANGLES, 0, n / 8);
        glBindVertexArray(0);
    }

    free(verts);
}


struct Chunk *world_adjacent_chunk(struct World *w, struct Chunk *c, vec3 dir)
{
    ivec2 idx = {
        (c->pos[0] - w->chunks[0][0]->pos[0]) / 16.f,
        (c->pos[2] - w->chunks[0][0]->pos[2]) / 16.f
    };

    int ix = idx[0] + dir[0];
    int iz = idx[1] + dir[2];

    if (ix < 0 || ix >= RENDER_DISTANCE || iz < 0 || iz >= RENDER_DISTANCE)
        return 0;

    return w->chunks[idx[0] + (int)dir[0]][idx[1] + (int)dir[2]];

    /* vec3 offset; */
    /* glm_vec3_scale(dir, 16.f, offset); */

    /* vec3 target; */
    /* glm_vec3_add(c->pos, offset, target); */

    /* for (size_t i = 0; i < w->nchunks; ++i) */
    /* { */
    /*     // Ignore y component */
    /*     target[1] = w->chunks[i]->pos[1]; */

    /*     if (glm_vec3_distance(target, w->chunks[i]->pos) < 1.f) */
    /*         return w->chunks[i]; */
    /* } */

    /* return 0; */
}


struct CubeTexture *world_get_tex(struct World *w, int block)
{
    if (block == 0)
        return 0;

    return w->texs[block - 1];
}


void world_init_renderer()
{
    glGenVertexArrays(1, &g_vao);
    glBindVertexArray(g_vao);

    glGenBuffers(1, &g_vb);
    glBindBuffer(GL_ARRAY_BUFFER, g_vb);
    glBufferData(GL_ARRAY_BUFFER, 0, 0, GL_DYNAMIC_DRAW);
    g_vb_size = 0;

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}


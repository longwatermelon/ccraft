#include "world.h"
#include <glad/glad.h>

unsigned int g_vao, g_vb;
size_t g_vb_size;

struct World *world_alloc()
{
    struct World *w = malloc(sizeof(struct World));

    w->nchunks = 36;
    w->chunks = malloc(sizeof(struct Chunk*) * w->nchunks);

    size_t idx = 0;

    for (int x = -48; x < 48; x += 16)
    {
        for (int z = -48; z < 48; z += 16)
        {
            w->chunks[idx++] = chunk_alloc(w, (vec3){ x, -15.f, z });
        }
    }

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
    mat4 model;
    shader_mat4(ri->shader, "view", ri->view);
    shader_mat4(ri->shader, "projection", ri->proj);

    for (size_t i = 0; i < w->nchunks; ++i)
    {
        glm_mat4_identity(model);
        glm_translate(model, w->chunks[i]->pos);
        shader_mat4(ri->shader, "model", model);

        world_render_side(w, ri, w->chunks[i], SIDE_TOP);
        world_render_side(w, ri, w->chunks[i], SIDE_SIDE);
        world_render_side(w, ri, w->chunks[i], SIDE_BOT);
    }
}


void world_render_side(struct World *w, RenderInfo *ri, struct Chunk *c, int side)
{
    size_t n;
    float *verts = chunk_visible_verts(c, side, ri->cam, &n);

    switch (side)
    {
    case SIDE_TOP: tex_bind(w->tex->top, 0); break;
    case SIDE_SIDE: tex_bind(w->tex->side, 0); break;
    case SIDE_BOT: tex_bind(w->tex->bottom, 0); break;
    }

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
    vec3 offset;
    glm_vec3_scale(dir, 16.f, offset);

    vec3 target;
    glm_vec3_add(c->pos, offset, target);

    for (size_t i = 0; i < w->nchunks; ++i)
    {
        // Ignore y component
        target[1] = w->chunks[i]->pos[1];

        if (glm_vec3_distance(target, w->chunks[i]->pos) < 1.f)
            return w->chunks[i];
    }

    return 0;
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


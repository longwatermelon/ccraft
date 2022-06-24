#include "world.h"
#include <glad/glad.h>

unsigned int g_vao, g_vb;

struct World *world_alloc()
{
    struct World *w = malloc(sizeof(struct World));
    w->chunks = malloc(sizeof(struct Chunk*) * 10);
    w->nchunks = 10;

    for (size_t i = 0; i < 10; ++i)
    {
        w->chunks[i] = chunk_alloc((vec3){ i * 16, -15.f, 0.f });
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
    float *verts = chunk_visible_verts(c, side, &n);

    switch (side)
    {
    case SIDE_TOP: tex_bind(w->tex->top, 0); break;
    case SIDE_SIDE: tex_bind(w->tex->side, 0); break;
    case SIDE_BOT: tex_bind(w->tex->bottom, 0); break;
    }

    glBindBuffer(GL_ARRAY_BUFFER, g_vb);
    glBufferSubData(GL_ARRAY_BUFFER, 0, n * sizeof(float), verts);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(g_vao);
    glDrawArrays(GL_TRIANGLES, 0, n / 8);
    glBindVertexArray(0);
}


void world_init_renderer()
{
    glGenVertexArrays(1, &g_vao);
    glBindVertexArray(g_vao);

    glGenBuffers(1, &g_vb);
    glBindBuffer(GL_ARRAY_BUFFER, g_vb);
    // Maximum buffer size (4 full sides)
    glBufferData(GL_ARRAY_BUFFER, 16 * 256 * 48 * sizeof(float), 0, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}


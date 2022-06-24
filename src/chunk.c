#include "chunk.h"
#include "shader.h"
#include <glad/glad.h>

float g_left[] = {
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f
};

float g_right[] = {
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f
};

float g_front[] = {
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f
};

float g_back[] = {
    0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
    0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
    0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
    0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
    0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
    0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f
};

float g_bottom[] = {
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f
};

float g_top[] = {
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f
};

unsigned int g_vao, g_vb;

struct CubeTexture *ct_alloc(const char *top, const char *bot, const char *side)
{
    struct CubeTexture *ct = malloc(sizeof(struct CubeTexture));
    ct->top = tex_alloc(top);
    ct->bottom = tex_alloc(bot);
    ct->side = tex_alloc(side);

    return ct;
}


void ct_free(struct CubeTexture *ct)
{
    tex_free(ct->top);
    tex_free(ct->bottom);
    tex_free(ct->side);
    free(ct);
}


struct Chunk *chunk_alloc(vec3 pos)
{
    struct Chunk *c = malloc(sizeof(struct Chunk));
    glm_vec3_copy(pos, c->pos);

    for (int x = 0; x < 16; ++x)
    {
        for (int y = 0; y < 256; ++y)
        {
            for (int z = 0; z < 16; ++z)
            {
                c->grid[x][y][z] = y < 10 ? 1 : 0;
            }
        }
    }

    return c;
}


void chunk_free(struct Chunk *c)
{
    free(c);
}


void chunk_render(struct Chunk *c, RenderInfo *ri, struct CubeTexture *tex)
{
    shader_mat4(ri->shader, "view", ri->view);
    shader_mat4(ri->shader, "projection", ri->proj);

    for (int x = 0; x < 16; ++x)
    {
        for (int y = 0; y < 256; ++y)
        {
            for (int z = 0; z < 16; ++z)
            {
                chunk_render_cube(c, ri, x, y, z, tex);
            }
        }
    }
}


void chunk_render_cube(struct Chunk *c, RenderInfo *ri, int x, int y, int z, struct CubeTexture *tex)
{
    if (!chunk_get(c, x, y, z))
        return;

    mat4 model;
    glm_mat4_identity(model);

    vec3 pos;
    glm_vec3_add(c->pos, (vec3){ x, y, z }, pos);

    glm_translate(model, pos);
    shader_mat4(ri->shader, "model", model);

    if (!chunk_get(c, x + 1, y, z)) chunk_render_face(c, ri, x, y, z, g_back, tex->side);
    if (!chunk_get(c, x - 1, y, z)) chunk_render_face(c, ri, x, y, z, g_front, tex->side);
    if (!chunk_get(c, x, y + 1, z)) chunk_render_face(c, ri, x, y, z, g_top, tex->top);
    if (!chunk_get(c, x, y - 1, z)) chunk_render_face(c, ri, x, y, z, g_bottom, tex->bottom);
    if (!chunk_get(c, x, y, z + 1)) chunk_render_face(c, ri, x, y, z, g_right, tex->side);
    if (!chunk_get(c, x, y, z - 1)) chunk_render_face(c, ri, x, y, z, g_left, tex->side);
}


void chunk_render_face(struct Chunk *c, RenderInfo *ri, int x, int y, int z, float *face, struct Texture *tex)
{
    tex_bind(tex, 0);

    glBindBuffer(GL_ARRAY_BUFFER, g_vb);
    glBufferSubData(GL_ARRAY_BUFFER, 0, 8 * 6 * sizeof(float), face);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(g_vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);
}


int chunk_get(struct Chunk *c, int x, int y, int z)
{
    if (x < 0 || x >= 16 || y < 0 || y >= 256 || z < 0 || z >= 16)
        return 0;

    return c->grid[x][y][z];
}


void chunk_init_renderer()
{
    glGenVertexArrays(1, &g_vao);
    glBindVertexArray(g_vao);

    glGenBuffers(1, &g_vb);
    glBindBuffer(GL_ARRAY_BUFFER, g_vb);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_top), 0, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}


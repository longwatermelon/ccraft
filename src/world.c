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

    for (int x = 0; x < RENDER_DISTANCE; ++x)
    {
        for (int z = 0; z < RENDER_DISTANCE; ++z)
        {
            chunk_update_blockstates(w->chunks[x][z]);
        }
    }

    w->ntexs = 2;
    w->texs = malloc(sizeof(struct CubeTexture*) * w->ntexs);
    w->texs[0] = ct_alloc((vec2){ 0.f, 0.f }, (vec2){ 200.f, 0.f }, (vec3){ 100.f, 0.f });
    w->texs[1] = ct_alloc((vec2){ 200.f, 0.f }, (vec2){ 200.f, 0.f }, (vec2){ 200.f, 0.f });

    w->atlas = tex_alloc("res/atlas.png");

    w->vertbuffer_size = 48000;
    w->vertbuffer = malloc(sizeof(float) * w->vertbuffer_size);

    return w;
}


void world_free(struct World *w)
{
    for (size_t i = 0; i < w->ntexs; ++i)
        ct_free(w->texs[i]);

    free(w->texs);

    tex_free(w->atlas);

    free(w->vertbuffer);

    free(w);
}


void world_render(struct World *w, RenderInfo *ri)
{
    shader_mat4(ri->shader, "view", ri->view);
    shader_mat4(ri->shader, "projection", ri->proj);

    tex_bind(w->atlas, 0);
    size_t count = 0;

    mat4 model;
    glm_mat4_identity(model);
    shader_mat4(ri->shader, "model", model);

    for (int x = 0; x < RENDER_DISTANCE; ++x)
    {
        for (int z = 0; z < RENDER_DISTANCE; ++z)
        {
            struct Chunk *chunk = w->chunks[x][z];
            chunk_visible_verts(chunk, ri->cam, &w->vertbuffer, &count, &w->vertbuffer_size);
        }
    }

    if (count)
    {
        glBindBuffer(GL_ARRAY_BUFFER, g_vb);

        if (count * sizeof(float) > g_vb_size)
        {
            glBufferData(GL_ARRAY_BUFFER, count * sizeof(float), w->vertbuffer, GL_DYNAMIC_DRAW);
            g_vb_size = count * sizeof(float);
        }
        else
        {
            glBufferSubData(GL_ARRAY_BUFFER, 0, count * sizeof(float), w->vertbuffer);
        }

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindVertexArray(g_vao);
        glDrawArrays(GL_TRIANGLES, 0, count / 8);
        glBindVertexArray(0);
    }
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


int world_get_block(struct World *w, vec3 pos, struct Chunk **chunk)
{
    /* int sigx = pos[0] < 0 ? -1 : 1; */
    /* int sigz = pos[2] < 0 ? -1 : 1; */

    vec3 center;
    world_center(w, center);

    vec3 fcoords;
    glm_vec3_sub(pos, center, fcoords);

    int sigx = fcoords[0] < 0 ? -1 : 1;
    int sigz = fcoords[2] < 0 ? -1 : 1;

    ivec3 coords = { fcoords[0] + .5f * sigx, pos[1], fcoords[2] + .5f * sigz };

    ivec3 idx = {
        coords[0] / 16 - (coords[0] < 0 && coords[0] % 16 != 0 ? 1 : 0),
        coords[1],
        coords[2] / 16 - (coords[2] < 0 && coords[2] % 16 != 0 ? 1 : 0)
    };

    struct Chunk *c = w->chunks[idx[0] + RENDER_DISTANCE / 2][idx[2] + RENDER_DISTANCE / 2];
    if (chunk) *chunk = c;

    ivec2 block_idx = {
        coords[0] - (coords[0] / 16 * 16 - ((coords[0] < 0 && coords[0] % 16 != 0) ? 16 : 0)),
        coords[2] - (coords[2] / 16 * 16 - ((coords[2] < 0 && coords[2] % 16 != 0) ? 16 : 0)),
    };

    return c->grid[block_idx[0]][coords[1]][block_idx[1]];
}


void world_center(struct World *w, vec3 dest)
{
    vec3 center = { 16.f, 0.f, 16.f };
    glm_vec3_addadd(w->chunks[RENDER_DISTANCE - 1][RENDER_DISTANCE - 1]->pos, w->chunks[0][0]->pos, center);
    glm_vec3_scale(center, .5f, center);

    glm_vec3_copy(center, dest);
}


void world_gen_chunks(struct World *w, vec3 cam)
{
    vec3 center;
    world_center(w, center);

    center[1] = cam[1];

    vec3 diff;
    glm_vec3_sub(cam, center, diff);

    diff[1] = 0.f;

    if (glm_vec3_distance(cam, center) > 16.f)
    {
        vec3 move = {
            diff[0] ? (diff[0] < 0 ? -16.f : 16.f) : 0,
            0.f,
            diff[2] ? (diff[2] < 0 ? -16.f : 16.f) : 0
        };

        for (int x = 0; x < RENDER_DISTANCE; ++x)
        {
            for (int z = 0; z < RENDER_DISTANCE; ++z)
            {
                glm_vec3_add(w->chunks[x][z]->pos, move, w->chunks[x][z]->pos);
                chunk_gen_terrain(w->chunks[x][z]);
            }
        }

        for (int x = 0; x < RENDER_DISTANCE; ++x)
        {
            for (int z = 0; z < RENDER_DISTANCE; ++z)
            {
                chunk_update_blockstates(w->chunks[x][z]);
            }
        }
    }
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


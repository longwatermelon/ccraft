#include "world.h"
#include "util.h"
#include "player.h"
#include <string.h>
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

    for (int x = -i; x < i; x += 16)
    {
        for (int z = -i; z < i; z += 16)
        {
            chunk_gen_trees(w->chunks[(x + i) / 16][(z + i) / 16]);
        }
    }

    for (int x = 0; x < RENDER_DISTANCE; ++x)
    {
        for (int z = 0; z < RENDER_DISTANCE; ++z)
        {
            chunk_update_blockstates(w->chunks[x][z]);
        }
    }

    world_connect_chunks(w);
    w->back_left = w->chunks[0][0];

    w->cz = RENDER_DISTANCE;
    w->cx = RENDER_DISTANCE;

    w->ntexs = 4;
    w->texs = malloc(sizeof(struct CubeTexture*) * w->ntexs);
    w->texs[0] = ct_alloc((vec2){ 0.f, 0.f }, (vec2){ 200.f, 0.f }, (vec3){ 100.f, 0.f });
    w->texs[1] = ct_alloc((vec2){ 200.f, 0.f }, (vec2){ 200.f, 0.f }, (vec2){ 200.f, 0.f });
    w->texs[2] = ct_alloc((vec2){ 300.f, 0.f }, (vec2){ 300.f, 0.f }, (vec2){ 400.f, 0.f });
    w->texs[3] = ct_alloc((vec2){ 0.f, 100.f }, (vec2){ 0.f, 100.f }, (vec2){ 0.f, 100.f });

    w->atlas = tex_alloc("res/atlas.png");

    w->vertbuffer_size = 72000;
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


void world_render(struct World *w, RenderInfo *ri, ivec3 selected)
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
            chunk_visible_verts(chunk, ri->cam, &w->vertbuffer, &count, &w->vertbuffer_size, selected);
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
        glDrawArrays(GL_TRIANGLES, 0, count / 12);
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

    return w->chunks[ix][iz];

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


int world_get_block(struct World *w, vec3 pos, struct Chunk **chunk, ivec3 index)
{
    /* int sigx = pos[0] < 0 ? -1 : 1; */
    /* int sigz = pos[2] < 0 ? -1 : 1; */

    vec3 center;
    world_center(w, center);

    vec3 fcoords;
    glm_vec3_sub(pos, center, fcoords);

    vec3 from_left_corner;
    glm_vec3_sub(pos, w->chunks[0][0]->pos, from_left_corner);

    ivec2 chunk_index = {
        (int)(from_left_corner[0] / 16.f),
        (int)(from_left_corner[2] / 16.f)
    };

    if (chunk_index[0] < 0 || chunk_index[0] >= RENDER_DISTANCE ||
        chunk_index[1] < 0 || chunk_index[1] >= RENDER_DISTANCE)
    {
        return 0;
    }

    struct Chunk *c = w->chunks[chunk_index[0]][chunk_index[1]];
    if (chunk) *chunk = c;

    ivec3 block_index = {
        (int)from_left_corner[0] - (chunk_index[0] * 16),
        (int)pos[1],
        (int)from_left_corner[2] - (chunk_index[1] * 16)
    };

    if (index)
        memcpy(index, block_index, 3 * sizeof(int));

    return chunk_get(c, block_index, true);

    /* ivec3 coords = { fcoords[0], pos[1], fcoords[2] }; */

    /* ivec3 idx = { */
    /*     coords[0] / 16 - (coords[0] < 0 && coords[0] % 16 != 0 ? 1 : 0), */
    /*     coords[1], */
    /*     coords[2] / 16 - (coords[2] < 0 && coords[2] % 16 != 0 ? 1 : 0) */
    /* }; */

    /* ivec2 chunk_index = { */
    /*     idx[0] + RENDER_DISTANCE / 2, */
    /*     idx[2] + RENDER_DISTANCE / 2 */
    /* }; */


    /* struct Chunk *c = w->chunks[chunk_index[0]][chunk_index[1]]; */
    /* if (chunk) *chunk = c; */

    /* ivec2 block_idx = { */
    /*     coords[0] - (coords[0] / 16 * 16 - ((coords[0] < 0 && coords[0] % 16 != 0) ? 16 : 0)), */
    /*     coords[2] - (coords[2] / 16 * 16 - ((coords[2] < 0 && coords[2] % 16 != 0) ? 16 : 0)), */
    /* }; */

    /* if (index) */
    /* { */
    /*     index[0] = block_idx[0]; */
    /*     index[1] = coords[1]; */
    /*     index[2] = block_idx[1]; */
    /* } */

    /* return c->grid[block_idx[0]][coords[1]][block_idx[1]]; */
}


float world_cast_ray(struct World *w, struct Camera *cam, struct Chunk **chunk, ivec3 coords, int *type)
{
    float tmp = cam->rot[2];
    cam->rot[2] = 2.f * M_PI - cam->rot[2];

    float cam_h = cam->pos[1];
    cam->pos[1] -= PLAYER_EYE_OFFSET;

    ivec3 x, y, z;
    struct Chunk *cx, *cy, *cz;

    float dx = world_cast_rayx(w, cam, &cx, x);
    float dy = world_cast_rayy(w, cam, &cy, y);
    float dz = world_cast_rayz(w, cam, &cz, z);

    /* dy = INFINITY; */
    /* dz = INFINITY; */

    cam->rot[2] = tmp;
    cam->pos[1] = cam_h;

    ivec3 *c;
    float min = dx;
    c = &x;
    *chunk = cx;
    *type = RAYI_X;

    if (dy < min)
    {
        min = dy;
        c = &y;
        *chunk = cy;
        *type = RAYI_Y;
    }

    if (dz < min)
    {
        min = dz;
        c = &z;
        *chunk = cz;
        *type = RAYI_Z;
    }

    memcpy(coords, *c, 3 * sizeof(int));
    return min;
}


float world_cast_rayx(struct World *w, struct Camera *cam, struct Chunk **c, ivec3 coords)
{
    vec3 ray;

    bool facing_forwards = cam->rot[2] > 3.f * M_PI / 2.f || cam->rot[2] < M_PI / 2.f;
    ray[0] = (int)cam->pos[0] + (facing_forwards ? 1 : 0);

    float a = (ray[0] - cam->pos[0]) / cosf(cam->rot[2]);
    ray[1] = cam->pos[1] + a * tanf(cam->rot[1]);
    ray[2] = cam->pos[2] + a * sinf(cam->rot[2]);

    int depth = 0;
    while (depth < 8)
    {
        if (!facing_forwards) ray[0] -= 1.f;

        if (world_get_block(w, ray, c, coords))
        {
            if (!facing_forwards) ray[0] += 1.f;
            return glm_vec3_distance(ray, cam->pos);
        }

        if (!facing_forwards) ray[0] += 1.f;

        ray[0] += facing_forwards ? 1 : -1;
        a = (ray[0] - cam->pos[0]) / cosf(cam->rot[2]);
        ray[1] = cam->pos[1] + a * tanf(cam->rot[1]);
        ray[2] = cam->pos[2] + a * sinf(cam->rot[2]);

        ++depth;
    }

    return INFINITY;
}


float world_cast_rayy(struct World *w, struct Camera *cam, struct Chunk **c, ivec3 coords)
{
    vec3 ray;

    bool facing_up = cam->rot[1] < M_PI / 2.f;
    ray[1] = (int)cam->pos[1] + (facing_up ? 1 : 0);

    float a = (ray[1] - cam->pos[1]) / tanf(cam->rot[1]);
    ray[0] = cam->pos[0] + a * cosf(cam->rot[2]);
    ray[2] = cam->pos[2] + a * sinf(cam->rot[2]);

    int depth = 0;
    while (depth < 8)
    {
        if (!facing_up) ray[1] -= 1.f;

        if (world_get_block(w, ray, c, coords))
        {
            if (!facing_up) ray[1] += 1.f;
            return glm_vec3_distance(ray, cam->pos);
        }

        if (!facing_up) ray[1] += 1.f;

        ray[1] += facing_up ? 1 : -1;
        a = (ray[1] - cam->pos[1]) / tanf(cam->rot[1]);
        ray[0] = cam->pos[0] + a * cosf(cam->rot[2]);
        ray[2] = cam->pos[2] + a * sinf(cam->rot[2]);

        ++depth;
    }

    return INFINITY;
}


float world_cast_rayz(struct World *w, struct Camera *cam, struct Chunk **c, ivec3 coords)
{
    vec3 ray;

    bool facing_right = cam->rot[2] < M_PI;
    ray[2] = (int)cam->pos[2] + (facing_right ? 1 : 0);

    float a = (ray[2] - cam->pos[2]) / sinf(cam->rot[2]);
    ray[0] = cam->pos[0] + a * cosf(cam->rot[2]);
    ray[1] = cam->pos[1] + a * tanf(cam->rot[1]);

    int depth = 0;
    while (depth < 8)
    {
        if (!facing_right) ray[2] -= 1.f;

        if (world_get_block(w, ray, c, coords))
        {
            if (!facing_right) ray[2] += 1.f;
            return glm_vec3_distance(ray, cam->pos);
        }

        if (!facing_right) ray[2] += 1.f;

        ray[2] += facing_right ? 1 : -1;
        a = (ray[2] - cam->pos[2]) / sinf(cam->rot[2]);
        ray[0] = cam->pos[0] + a * cosf(cam->rot[2]);
        ray[1] = cam->pos[1] + a * tanf(cam->rot[1]);

        ++depth;
    }

    return INFINITY;
}


void world_center(struct World *w, vec3 dest)
{
    struct Chunk *c = w->chunks[RENDER_DISTANCE / 2][RENDER_DISTANCE / 2];

    glm_vec3_copy((vec3){
        c->pos[0] + 8.f,
        0.f,
        c->pos[2] + 8.f
    }, dest);
}


void world_gen_chunks(struct World *w, vec3 cam)
{
    vec3 center;
    world_center(w, center);

    center[1] = cam[1];

    vec3 diff;
    glm_vec3_sub(cam, center, diff);

    diff[1] = 0.f;

    /* if (glm_vec3_distance(cam, center) > 16.f) */
    /* { */
    struct Chunk *back_left = w->chunks[0][0];
    bool shifted = false;
    bool f = false, b = false, l = false, r = false;

    if (diff[0] > 8.f)
    {
        shifted = true;
        f = true;
        world_gen_chunks_front(w);
        back_left = back_left->front;
    }

    if (diff[0] < -8.f)
    {
        shifted = true;
        b = true;
        world_gen_chunks_back(w);
        back_left = back_left->back;
    }

    if (diff[2] < -8.f)
    {
        shifted = true;
        l = true;
        world_gen_chunks_left(w);
        back_left = back_left->left;
    }

    if (diff[2] > 8.f)
    {
        shifted = true;
        r = true;
        world_gen_chunks_right(w);
        back_left = back_left->right;
    }

    if (shifted)
    {
        world_fill_chunk_array(w, back_left);

        for (int x = 0; x < RENDER_DISTANCE; ++x)
        {
            struct Chunk *left = w->chunks[x][0];
            struct Chunk *right = w->chunks[x][RENDER_DISTANCE - 1];

            if (l)
            {
                chunk_gen_trees(left);
                chunk_update_blockstates(left);
                chunk_update_blockstates(left->right);
            }

            if (r)
            {
                chunk_gen_trees(right);
                chunk_update_blockstates(right);
                chunk_update_blockstates(right->left);
            }
        }

        for (int z = 0; z < RENDER_DISTANCE; ++z)
        {
            struct Chunk *front = w->chunks[RENDER_DISTANCE - 1][z];
            struct Chunk *back = w->chunks[0][z];

            if (f)
            {
                chunk_gen_trees(front);
                chunk_update_blockstates(front);
                chunk_update_blockstates(front->back);
            }

            if (b)
            {
                chunk_gen_trees(back);
                chunk_update_blockstates(back);
                chunk_update_blockstates(back->front);
            }
        }

        /* for (int x = 0; x < RENDER_DISTANCE; ++x) */
        /* { */
        /*     for (int z = 0; z < RENDER_DISTANCE; ++z) */
        /*     { */
        /*         chunk_update_blockstates(w->chunks[x][z]); */
        /*     } */
        /* } */
    }

        /* vec3 move = { */
        /*     diff[0] ? (diff[0] < 0 ? -16.f : 16.f) : 0, */
        /*     0.f, */
        /*     diff[2] ? (diff[2] < 0 ? -16.f : 16.f) : 0 */
        /* }; */

        /* for (int x = 0; x < RENDER_DISTANCE; ++x) */
        /* { */
        /*     for (int z = 0; z < RENDER_DISTANCE; ++z) */
        /*     { */
        /*         glm_vec3_add(w->chunks[x][z]->pos, move, w->chunks[x][z]->pos); */
        /*         chunk_gen_terrain(w->chunks[x][z]); */
        /*     } */
        /* } */

        /* for (int x = 0; x < RENDER_DISTANCE; ++x) */
        /* { */
        /*     for (int z = 0; z < RENDER_DISTANCE; ++z) */
        /*     { */
        /*         chunk_update_blockstates(w->chunks[x][z]); */
        /*     } */
        /* } */
    /* } */
}


void world_gen_chunks_front(struct World *w)
{
    for (size_t z = 0; z < w->cz; ++z)
    {
        struct Chunk *c = world_get_chunk(w, w->cx - 1, z);

        if (!c->front)
        {
            c->front = chunk_alloc(w, (vec3){
                c->pos[0] + 16.f,
                c->pos[1],
                c->pos[2]
            });
        }
    }

    for (size_t z = 0; z < w->cz; ++z)
    {
        struct Chunk *c = world_get_chunk(w, w->cx - 1, z);

        c->front->back = c;
        if (c->left) c->front->left = c->left->front;
        if (c->right) c->front->right = c->right->front;
    }

    ++w->cx;
}


void world_gen_chunks_back(struct World *w)
{
    for (size_t z = 0; z < w->cz; ++z)
    {
        struct Chunk *c = world_get_chunk(w, 0, z);

        if (!c->back)
        {
            c->back = chunk_alloc(w, (vec3){
                c->pos[0] - 16.f,
                c->pos[1],
                c->pos[2]
            });
        }
    }

    for (size_t z = 0; z < w->cz; ++z)
    {
        struct Chunk *c = world_get_chunk(w, 0, z);

        c->back->front = c;
        if (c->left) c->back->left = c->left->back;
        if (c->right) c->back->right = c->right->back;
    }

    ++w->cx;
    w->back_left = w->back_left->back;
}


void world_gen_chunks_left(struct World *w)
{
    for (size_t x = 0; x < w->cx; ++x)
    {
        struct Chunk *c = world_get_chunk(w, x, 0);

        if (!c->left)
        {
            c->left = chunk_alloc(w, (vec3){
                c->pos[0],
                c->pos[1],
                c->pos[2] - 16.f
            });
        }
    }

    for (size_t x = 0; x < w->cx; ++x)
    {
        struct Chunk *c = world_get_chunk(w, x, 0);

        c->left->right = c;
        if (c->front) c->left->front = c->front->left;
        if (c->back) c->left->back = c->back->left;
    }

    ++w->cz;
    w->back_left = w->back_left->left;
}


void world_gen_chunks_right(struct World *w)
{
    for (size_t x = 0; x < w->cx; ++x)
    {
        struct Chunk *c = world_get_chunk(w, x, w->cz - 1);

        if (!c->right)
        {
            c->right = chunk_alloc(w, (vec3){
                c->pos[0],
                c->pos[1],
                c->pos[2] + 16.f
            });
        }
    }

    for (size_t x = 0; x < w->cx; ++x)
    {
        struct Chunk *c = world_get_chunk(w, x, w->cz - 1);

        c->right->left = c;
        if (c->front) c->right->front = c->front->right;
        if (c->back) c->right->back = c->back->right;
    }

    ++w->cz;
}


void world_connect_chunks(struct World *w)
{
    for (int x = 0; x < RENDER_DISTANCE; ++x)
    {
        for (int z = 0; z < RENDER_DISTANCE; ++z)
        {
            if (x > 0)                    w->chunks[x][z]->back = w->chunks[x - 1][z];
            if (x < RENDER_DISTANCE - 1)  w->chunks[x][z]->front = w->chunks[x + 1][z];
            if (z > 0)                    w->chunks[x][z]->left = w->chunks[x][z - 1];
            if (z < RENDER_DISTANCE - 1)  w->chunks[x][z]->right = w->chunks[x][z + 1];
        }
    }
}


void world_fill_chunk_array(struct World *w, struct Chunk *back_left)
{
    struct Chunk *xbuf = back_left;

    for (int x = 0; x < RENDER_DISTANCE; ++x)
    {
        if (x > 0 && x < RENDER_DISTANCE - 1) xbuf = xbuf->front;
        struct Chunk *zbuf = xbuf;

        for (int z = 0; z < RENDER_DISTANCE; ++z)
        {
            if (z > 0 && z < RENDER_DISTANCE - 1) zbuf = zbuf->right;
            w->chunks[x][z] = zbuf;
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

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(3);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}


struct Chunk *world_get_chunk(struct World *w, int x, int z)
{
    struct Chunk *c = w->back_left;

    for (int i = 0; i < x; ++i)
    {
        c = c->front;
        if (!c) return 0;
    }

    for (int i = 0; i < z; ++i)
    {
        c = c->right;
        if (!c) return 0;
    }

    return c;
}


#include "chunk.h"
#include "shader.h"
#include "util.h"
#include "world.h"
#include <string.h>
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


struct Chunk *chunk_alloc(struct World *w, vec3 pos)
{
    struct Chunk *c = malloc(sizeof(struct Chunk));
    glm_vec3_copy(pos, c->pos);
    c->world = w;

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


float *chunk_visible_verts(struct Chunk *c, int side, size_t *n)
{
    float *verts = 0;
    *n = 0;

    size_t n2 = sizeof(g_top) / sizeof(float);

    for (int x = 0; x < 16; ++x)
    {
        for (int y = 0; y < 256; ++y)
        {
            for (int z = 0; z < 16; ++z)
            {
                if (!chunk_get(c, (ivec3){ x, y, z }))
                    continue;

                float face[48];
                ivec3 pos = { x, y, z };

                switch (side)
                {
                case SIDE_TOP:
                    if (!chunk_get(c, (ivec3){ x, y + 1, z }))
                    {
                        chunk_face_at(c, pos, g_top, face);
                        ARR_APPEND(verts, *n, face, n2, float);
                    }
                    break;
                case SIDE_BOT:
                    if (!chunk_get(c, (ivec3){ x, y - 1, z }))
                    {
                        chunk_face_at(c, pos, g_bottom, face);
                        ARR_APPEND(verts, *n, face, n2, float);
                    }
                    break;
                case SIDE_SIDE:
                {
                    bool append = false;

                    if (!chunk_get(c, (ivec3){ x + 1, y, z })) { chunk_face_at(c, pos, g_back, face); append = true; }
                    if (!chunk_get(c, (ivec3){ x - 1, y, z })) { chunk_face_at(c, pos, g_front, face); append = true; }
                    if (!chunk_get(c, (ivec3){ x, y, z + 1 })) { chunk_face_at(c, pos, g_right, face); append = true; }
                    if (!chunk_get(c, (ivec3){ x, y, z - 1 })) { chunk_face_at(c, pos, g_left, face); append = true; }

                    if (append)
                        ARR_APPEND(verts, *n, face, n2, float);
                } break;
                }
            }
        }
    }

    return verts;
}


void chunk_face_at(struct Chunk *c, ivec3 pos, float *face, float dest[48])
{
    memcpy(dest, face, 48 * sizeof(float));

    for (int i = 0; i < 48; i += 8)
    {
        dest[i] += pos[0];
        dest[i + 1] += pos[1];
        dest[i + 2] += pos[2];
    }
}


int chunk_get(struct Chunk *c, ivec3 pos)
{
    if (pos[1] < 0 || pos[1] >= 256)
        return 0;

    bool x = pos[0] < 0 || pos[0] >= 16;
    bool z = pos[2] < 0 || pos[2] >= 16;

    if (x || z)
    {
        int sigx = x ? (pos[0] < 0 ? -1 : 1) : 0;
        int sigz = z ? (pos[2] < 0 ? -1 : 1) : 0;

        vec3 dir = { sigx, 0.f, sigz };
        struct Chunk *adjacent = world_adjacent_chunk(c->world, c, dir);

        if (adjacent)
        {
            int ix = x ? (pos[0] - 16 + (sigx < 0 ? 32 : 0)) : pos[0];
            int iz = z ? (pos[2] - 16 + (sigz < 0 ? 32 : 0)) : pos[2];

            return adjacent->grid[ix][pos[1]][iz];
        }
        else
        {
            return 0;
        }
    }

    return c->grid[pos[0]][pos[1]][pos[2]];
}


#include "chunk.h"
#include "shader.h"
#include "util.h"
#include "world.h"
#include <string.h>
#include <glad/glad.h>
#include <noise/noise.h>

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

struct CubeTexture *ct_alloc(vec2 top, vec2 bot, vec2 side)
{
    struct CubeTexture *ct = malloc(sizeof(struct CubeTexture));
    glm_vec2_divs(top, 500.f, ct->top);
    glm_vec2_divs(bot, 500.f, ct->bottom);
    glm_vec2_divs(side, 500.f, ct->side);

    return ct;
}


void ct_free(struct CubeTexture *ct)
{
    free(ct);
}


struct Chunk *chunk_alloc(struct World *w, vec3 pos)
{
    struct Chunk *c = malloc(sizeof(struct Chunk));
    glm_vec3_copy(pos, c->pos);
    c->world = w;

    for (int x = 0; x < 16; ++x)
    {
        for (int z = 0; z < 16; ++z)
        {
            float res = simplex2((c->pos[0] + x) * .01f, (c->pos[2] + z) * .01f, 8, .6f, 1.f);
            c->heightmap[x][z] = 15.f + res * 150.f;
        }
    }

    for (int y = 0; y < 256; ++y)
    {
        for (int x = 0; x < 16; ++x)
        {
            for (int z = 0; z < 16; ++z)
            {
                if (y == c->heightmap[x][z])
                    c->grid[x][y][z] = BLOCK_GRASS;
                else if (y < c->heightmap[x][z])
                    c->grid[x][y][z] = BLOCK_DIRT;
                else
                    c->grid[x][y][z] = BLOCK_AIR;
            }
        }
    }

    return c;
}


void chunk_free(struct Chunk *c)
{
    free(c);
}


size_t chunk_visible_verts(struct Chunk *c, int side, struct Camera *cam, float **vertbuffer, size_t *n)
{
    size_t counter = 0;

    vec3 cpos;
    glm_vec3_sub(cam->pos, c->pos, cpos);

    for (int x = 0; x < 16; ++x)
    {
        for (int z = 0; z < 16; ++z)
        {
            for (int y = 0; y <= c->heightmap[x][z]; ++y)
            {
                if (c->block_states[x][y][z] == BSTATE_ENCLOSED)
                    continue;

                ivec3 pos = { x, y, z };

                switch (side)
                {
                case SIDE_TOP:
                    if (y < cpos[1])
                    {
                        if (!chunk_get(c, (ivec3){ x, y + 1, z }, false))
                            chunk_face_at(c, pos, vertbuffer, &counter, n, g_top);
                    }
                    break;
                case SIDE_BOT:
                    if (y > cpos[1])
                    {
                        if (!chunk_get(c, (ivec3){ x, y - 1, z }, false))
                            chunk_face_at(c, pos, vertbuffer, &counter, n, g_bottom);
                    }
                    break;
                case SIDE_SIDE:
                {
                    if (x < cpos[0])
                    {
                        if (!chunk_get(c, (ivec3){ x + 1, y, z }, false))
                            chunk_face_at(c, pos, vertbuffer, &counter, n, g_back);
                    }

                    if (x > cpos[0])
                    {
                        if (!chunk_get(c, (ivec3){ x - 1, y, z }, false))
                            chunk_face_at(c, pos, vertbuffer, &counter, n, g_front);
                    }

                    if (z < cpos[2])
                    {
                        if (!chunk_get(c, (ivec3){ x, y, z + 1 }, false))
                            chunk_face_at(c, pos, vertbuffer, &counter, n, g_right);
                    }

                    if (z > cpos[2])
                    {
                        if (!chunk_get(c, (ivec3){ x, y, z - 1 }, false))
                            chunk_face_at(c, pos, vertbuffer, &counter, n, g_left);
                    }
                } break;
                }
            }
        }
    }

    return counter;
}


void chunk_face_at(struct Chunk *c, ivec3 pos, float **verts, size_t *nverts, size_t *capacity, float *face)
{
    if (*nverts + 48 >= *capacity)
    {
        *capacity += 48000;
        *verts = realloc(*verts, sizeof(float) * *capacity);
    }

    float *arr = *verts;
    memcpy(&arr[*nverts], face, 48 * sizeof(float));

    int block = c->grid[pos[0]][pos[1]][pos[2]];
    struct CubeTexture *tex = world_get_tex(c->world, block);

    vec2 coords;

    if (face == g_top)          glm_vec2_copy(tex->top, coords);
    else if (face == g_bottom)  glm_vec2_copy(tex->bottom, coords);
    else                        glm_vec2_copy(tex->side, coords);

    float adjust = 1.f / 500.f;

    for (size_t i = *nverts; i < *nverts + 48; i += 8)
    {
        // Position
        arr[i] += pos[0];
        arr[i + 1] += pos[1];
        arr[i + 2] += pos[2];

        // Texture coords
        arr[i + 6] = arr[i + 6] ? coords[0] + 100.f / 500.f - adjust : coords[0] + adjust;
        arr[i + 7] = arr[i + 7] ? coords[1] + 100.f / 500.f - adjust : coords[1] + adjust;
    }

    *nverts += 48;
}


int chunk_get(struct Chunk *c, ivec3 pos, bool check_adjacent)
{
    if (check_adjacent)
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
                int ix = x ? (pos[0] - sigx * 16) : pos[0];
                int iz = z ? (pos[2] - sigz * 16) : pos[2];

                return adjacent->grid[ix][pos[1]][iz];
            }
            else
            {
                return 0;
            }
        }
    }
    else
    {
        if (pos[0] < 0 || pos[0] > 15 || pos[1] < 0 || pos[1] > 255 || pos[2] < 0 || pos[2] > 15)
            return 0;
    }

    return c->grid[pos[0]][pos[1]][pos[2]];
}


void chunk_find_highest(struct Chunk *c)
{
    for (int x = 0; x < 16; ++x)
    {
        for (int z = 0; z < 16; ++z)
        {
            c->heightmap[x][z] = 0;
        }
    }

    for (int y = 255; y >= 0; --y)
    {
        for (int x = 0; x < 16; ++x)
        {
            for (int z = 0; z < 16; ++z)
            {
                if (c->grid[x][y][z] && y > c->heightmap[x][z])
                    c->heightmap[x][z] = y;
            }
        }
    }
}


void chunk_update_blockstates(struct Chunk *c)
{
    for (int x = 0; x < 16; ++x)
    {
        for (int y = 0; y < 256; ++y)
        {
            for (int z = 0; z < 16; ++z)
            {
                if (chunk_get(c, (ivec3){ x - 1, y, z }, true) &&
                    chunk_get(c, (ivec3){ x + 1, y, z }, true) &&
                    chunk_get(c, (ivec3){ x, y - 1, z }, true) &&
                    chunk_get(c, (ivec3){ x, y + 1, z }, true) &&
                    chunk_get(c, (ivec3){ x, y, z - 1 }, true) &&
                    chunk_get(c, (ivec3){ x, y, z + 1 }, true))
                {
                    c->block_states[x][y][z] = BSTATE_ENCLOSED;
                }
                else
                {
                    c->block_states[x][y][z] = BSTATE_OPEN;
                }
            }
        }
    }
}


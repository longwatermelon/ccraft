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

struct CubeTexture *ct_alloc(vec2 top, vec2 bot, vec2 side)
{
    struct CubeTexture *ct = malloc(sizeof(struct CubeTexture));
    glm_vec2_copy(top, ct->top);
    glm_vec2_copy(bot, ct->bottom);
    glm_vec2_copy(side, ct->side);

    return ct;
}


void ct_free(struct CubeTexture *ct)
{
    free(ct);
}


void ct_normalize(struct CubeTexture *ct, vec2 textures[3])
{
    glm_vec2_divs(ct->top, 500.f, textures[0]);
    glm_vec2_divs(ct->bottom, 500.f, textures[1]);
    glm_vec2_divs(ct->side, 500.f, textures[2]);
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
                /* c->grid[x][y][z] = 1 ? (y < 30 ? 1 : 0) : 0; */
                c->grid[x][y][z] = x % 2 == 0 && z % 2 == 0 && y < 15 ? (y == 14 ? BLOCK_GRASS : BLOCK_DIRT) : BLOCK_AIR;
            }
        }
    }

    chunk_find_highest(c);

    return c;
}


void chunk_free(struct Chunk *c)
{
    free(c);
}


float *chunk_visible_verts(struct Chunk *c, int side, struct Camera *cam, size_t *n)
{
    float *verts = 0;
    *n = 0;

    size_t n2 = sizeof(g_top) / sizeof(float);
    vec3 cpos;
    glm_vec3_sub(cam->pos, c->pos, cpos);

    for (int x = 0; x < 16; ++x)
    {
        for (int z = 0; z < 16; ++z)
        {
            for (int y = 0; y <= c->heightmap[x][z]; ++y)
            {
                if (!chunk_get(c, (ivec3){ x, y, z }))
                    continue;

                float face[48];
                ivec3 pos = { x, y, z };

                switch (side)
                {
                case SIDE_TOP:
                    if (y < cpos[1])
                    {
                        if (!chunk_get(c, (ivec3){ x, y + 1, z }))
                        {
                            chunk_face_at(c, pos, g_top, face);
                            ARR_APPEND(verts, *n, face, n2, float);
                        }
                    }
                    break;
                case SIDE_BOT:
                    if (y > cpos[1])
                    {
                        if (!chunk_get(c, (ivec3){ x, y - 1, z }))
                        {
                            chunk_face_at(c, pos, g_bottom, face);
                            ARR_APPEND(verts, *n, face, n2, float);
                        }
                    }
                    break;
                case SIDE_SIDE:
                {
                    if (x < cpos[0])
                    {
                        if (!chunk_get(c, (ivec3){ x + 1, y, z }))
                        {
                            chunk_face_at(c, pos, g_back, face);
                            ARR_APPEND(verts, *n, face, n2, float);
                        }
                    }

                    if (x > cpos[0])
                    {
                        if (!chunk_get(c, (ivec3){ x - 1, y, z }))
                        {
                            chunk_face_at(c, pos, g_front, face);
                            ARR_APPEND(verts, *n, face, n2, float);
                        }
                    }

                    if (z < cpos[2])
                    {
                        if (!chunk_get(c, (ivec3){ x, y, z + 1 }))
                        {
                            chunk_face_at(c, pos, g_right, face);
                            ARR_APPEND(verts, *n, face, n2, float);
                        }
                    }

                    if (z > cpos[2])
                    {
                        if (!chunk_get(c, (ivec3){ x, y, z - 1 }))
                        {
                            chunk_face_at(c, pos, g_left, face);
                            ARR_APPEND(verts, *n, face, n2, float);
                        }
                    }
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

    int block = c->grid[pos[0]][pos[1]][pos[2]];
    struct CubeTexture *tex = world_get_tex(c->world, block);

    vec2 texcoords[3];
    ct_normalize(tex, texcoords);

    for (int i = 0; i < 48; i += 8)
    {
        // Position
        dest[i] += pos[0];
        dest[i + 1] += pos[1];
        dest[i + 2] += pos[2];

        // Texture coords
        vec2 coords;

        if (face == g_top)          glm_vec2_copy(texcoords[0], coords);
        else if (face == g_bottom)  glm_vec2_copy(texcoords[1], coords);
        else                        glm_vec2_copy(texcoords[2], coords);

        dest[i + 6] = dest[i + 6] ? coords[0] + 100.f / 500.f - 1.f / 500.f : coords[0] + 1.f / 500.f;
        dest[i + 7] = dest[i + 7] ? coords[1] + 100.f / 500.f - 1.f / 500.f : coords[1] + 1.f / 500.f;
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


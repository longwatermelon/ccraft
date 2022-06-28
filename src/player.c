#include "player.h"


struct Player *player_alloc()
{
    struct Player *p = malloc(sizeof(struct Player));
    p->cam = cam_alloc((vec3){ 0.f, 100.f, 0.f }, (vec3){ 0.f, 0.f, 0.f });
    glm_vec3_copy((vec3){ 0.f, 0.f, 0.f }, p->vel);

    return p;
}


void player_free(struct Player *p)
{
    cam_free(p->cam);
    free(p);
}


void player_update(struct Player *p, struct World *w)
{
    p->vel[1] -= .01f;

    vec3 pos;
    glm_vec3_copy(p->cam->pos, pos);

    player_check_collisions(p, w, pos);
    glm_vec3_add(p->cam->pos, p->vel, pos);

    glm_vec3_copy(pos, p->cam->pos);
}


void player_check_collisions(struct Player *p, struct World *w, vec3 pos)
{
    // Vertical collision
    if (p->vel[1] > 0.f)
    {
        int block = world_get_block(w, pos, 0);

        if (block != BLOCK_AIR)
        {
            pos[1] = (int)pos[1];
            p->vel[1] = 0.f;
        }
    }

    if (p->vel[1] < 0.f)
    {
        pos[1] -= 1.f;
        int block = world_get_block(w, pos, 0);
        pos[1] += 1.f;

        if (block != BLOCK_AIR)
        {
            pos[1] = (int)pos[1] + 1;
            p->vel[1] = 0.f;
        }
    }


    // Horizontal collision x
    pos[0] += p->vel[0];
    int block = world_get_block(w, pos, 0);

    int sigx = pos[0] < 0 ? -1 : 1;
    /* int sigz = pos[2] < 0 ? -1 : 1; */
    /* ivec3 coords = { pos[0] + .5f * sigx, pos[1], pos[2] + .5f * sigz }; */
    int xcoord = pos[0] + .5f * sigx;

    pos[1] -= 1.f;
    int block_bottom = world_get_block(w, pos, 0);
    pos[1] += 1.f;

    pos[0] -= p->vel[0];

    if (block != BLOCK_AIR || block_bottom != BLOCK_AIR)
    {
        if (p->vel[0] > 0.f)
            pos[0] = xcoord - .5f;
        else if (p->vel[0] < 0.f)
            pos[0] = xcoord + .5f;

        p->vel[0] = 0.f;
    }

    // Horizontal collision z
    pos[2] += p->vel[2];
    block = world_get_block(w, pos, 0);

    int sigz = pos[2] < 0 ? -1 : 1;
    int zcoord = pos[2] + .5f * sigz;

    pos[1] -= 1.f;
    block_bottom = world_get_block(w, pos, 0);
    pos[1] += 1.f;

    pos[2] -= p->vel[2];

    if (block != BLOCK_AIR || block_bottom != BLOCK_AIR)
    {
        if (p->vel[2] > 0.f)
            pos[2] = zcoord - .5f;
        else if (p->vel[2] < 0.f)
            pos[2] = zcoord + .5f;

        p->vel[2] = 0.f;
    }
}


void player_set_props(struct Player *p, unsigned int shader)
{
    cam_set_props(p->cam, shader);
}


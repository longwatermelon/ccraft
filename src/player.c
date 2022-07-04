#include "player.h"


struct Player *player_alloc()
{
    struct Player *p = malloc(sizeof(struct Player));
    p->cam = cam_alloc((vec3){ 0.f, 50.f, 0.f }, (vec3){ 0.f, 0.f, 0.f });
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
    /* if (p->vel[1] > 0.f) */
    /* { */
    /*     int block = world_get_block(w, pos, 0, 0); */

    /*     if (block != BLOCK_AIR) */
    /*     { */
    /*         pos[1] = (int)pos[1]; */
    /*         p->vel[1] = 0.f; */
    /*     } */
    /* } */

    if (p->vel[1] < 0.f)
    {
        float prev = pos[1];
        pos[1] += p->vel[1] - 2;

        ivec3 coords;
        int block = world_get_block(w, pos, 0, coords);

        pos[1] = prev;

        if (block != BLOCK_AIR)
        {
            pos[1] = coords[1] + 2.f;
            p->vel[1] = 0.f;
        }
    }

    // Horizontal collision x
    int sigx = p->vel[0] < 0 ? -1 : 1;

    pos[0] += p->vel[0] + .1f * sigx;
    int block = world_get_block(w, pos, 0, 0);

    int xcoord = pos[0];

    pos[1] -= 1.f;
    int block_bottom = world_get_block(w, pos, 0, 0);
    pos[1] += 1.f;

    pos[0] -= p->vel[0];

    if (block != BLOCK_AIR || block_bottom != BLOCK_AIR)
    {
        if (p->vel[0] > 0.f)
            pos[0] = xcoord - .1f;
        else if (p->vel[0] < 0.f)
            pos[0] = xcoord + 1.1f;

        p->vel[0] = 0.f;
    }

    // Horizontal collision z
    int sigz = p->vel[2] < 0 ? -1 : 1;

    pos[2] += p->vel[2] + .1f * sigz;
    block = world_get_block(w, pos, 0, 0);

    int zcoord = pos[2];

    pos[1] -= 1.f;
    block_bottom = world_get_block(w, pos, 0, 0);
    pos[1] += 1.f;

    pos[2] -= p->vel[2];

    if (block != BLOCK_AIR || block_bottom != BLOCK_AIR)
    {
        if (p->vel[2] > 0.f)
            pos[2] = zcoord - .1f;
        else if (p->vel[2] < 0.f)
            pos[2] = zcoord + 1.1f;

        p->vel[2] = 0.f;
    }
}


void player_set_props(struct Player *p, unsigned int shader)
{
    cam_set_props(p->cam, shader);
}


void player_view(struct Player *p, mat4 dest)
{
    float prev = p->cam->pos[1];
    p->cam->pos[1] -= PLAYER_EYE_OFFSET;
    cam_view_mat(p->cam, dest);
    p->cam->pos[1] = prev;
}


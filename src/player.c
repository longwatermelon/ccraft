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
    glm_vec3_add(p->cam->pos, p->vel, pos);

    pos[1] -= 1.f;
    int block = world_get_block(w, pos, 0);
    pos[1] += 1.f;

    if (block != BLOCK_AIR)
    {
        pos[1] = (int)pos[1] + 1;
        p->vel[1] = 0.f;
    }

    glm_vec3_copy(pos, p->cam->pos);
}


void player_set_props(struct Player *p, unsigned int shader)
{
    cam_set_props(p->cam, shader);
}


#ifndef PLAYER_H
#define PLAYER_H

#include "camera.h"
#include "world.h"

struct Player
{
    struct Camera *cam;
    vec3 vel;
};

struct Player *player_alloc();
void player_free(struct Player *p);

void player_update(struct Player *p, struct World *w);

void player_set_props(struct Player *p, unsigned int shader);

#endif

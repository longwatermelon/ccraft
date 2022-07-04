#ifndef PLAYER_H
#define PLAYER_H

#include "camera.h"
#include "world.h"

#define PLAYER_REACH 8.f
#define PLAYER_EYE_OFFSET .5f

struct Player
{
    struct Camera *cam;
    vec3 vel;
};

struct Player *player_alloc();
void player_free(struct Player *p);

void player_update(struct Player *p, struct World *w);
// Return pos
void player_check_collisions(struct Player *p, struct World *w, vec3 pos);

void player_destroy_block(struct Player *p, struct World *w);
void player_place_block(struct Player *p, struct World *w);

void player_set_props(struct Player *p, unsigned int shader);

// View matrix slightly lower than cam pos[1], eyes don't exist on the top of your head
void player_view(struct Player *p, mat4 dest);

#endif


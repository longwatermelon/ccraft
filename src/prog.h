#ifndef PROG_H
#define PROG_H

#include "shader.h"
#include "render.h"
#include "player.h"
#include "skybox.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

struct Prog
{
    GLFWwindow *win;
    RenderInfo *ri;

    struct Player *player;

    struct Skybox *skybox;
};

struct Prog *prog_alloc(GLFWwindow *win);
void prog_free(struct Prog *p);

void prog_mainloop(struct Prog *p);

void prog_events(struct Prog *p);

#endif


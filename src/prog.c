#include "prog.h"
#include "chunk.h"
#include "world.h"
#include "util.h"
#include <stb/stb_image.h>
#include <stdlib.h>


struct Prog *prog_alloc(GLFWwindow *win)
{
    struct Prog *p = malloc(sizeof(struct Prog));
    p->win = win;

    p->cam = cam_alloc((vec3){ 0.f, 15.f, 0.f }, (vec3){ 0.f, 0.f, 0.f });

    p->ri = ri_alloc();
    ri_add_shader(p->ri, "shaders/basic_v.glsl", "shaders/basic_f.glsl");

    p->ri->cam = p->cam;

    return p;
}


void prog_free(struct Prog *p)
{
    cam_free(p->cam);
    free(p);
}


void prog_mainloop(struct Prog *p)
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glfwSetCursorPos(p->win, 400.f, 300.f);
    glfwSetInputMode(p->win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    double prev_mx, prev_my;
    glfwGetCursorPos(p->win, &prev_mx, &prev_my);

    struct World *w = world_alloc();

    /* float prev = glfwGetTime(); */

    while (!glfwWindowShouldClose(p->win))
    {
        /* printf("%f\n", 1.f / (glfwGetTime() - prev)); */
        /* prev = glfwGetTime(); */

        double mx, my;
        glfwGetCursorPos(p->win, &mx, &my);

        cam_rot(p->cam, (vec3){ 0.f, -(my - prev_my) / 100.f, -(mx - prev_mx) / 100.f });
        prev_mx = mx;
        prev_my = my;

        prog_events(p);

        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ri_use_shader(p->ri, SHADER_BASIC);

        cam_set_props(p->cam, p->ri->shader);
        cam_view_mat(p->cam, p->ri->view);

        /* glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); */
        world_render(w, p->ri);

        glfwSwapBuffers(p->win);
        glfwPollEvents();
    }

    world_free(w);
}


void prog_events(struct Prog *p)
{
    float move = .1f;

    vec3 angle;
    glm_vec3_copy(p->cam->rot, angle);
    angle[1] = 0.f;

    vec4 quat;
    util_quat_from_rot(angle, quat);

    vec3 front = { 1.f, 0.f, 0.f };
    vec3 right = { 0.f, 0.f, 1.f };
    glm_quat_rotatev(quat, front, front);
    glm_quat_rotatev(quat, right, right);

    glm_vec3_scale(front, move, front);
    glm_vec3_scale(right, move, right);

    if (glfwGetKey(p->win, GLFW_KEY_W) == GLFW_PRESS) glm_vec3_add(p->cam->pos, front, p->cam->pos);
    if (glfwGetKey(p->win, GLFW_KEY_S) == GLFW_PRESS) glm_vec3_sub(p->cam->pos, front, p->cam->pos);
    if (glfwGetKey(p->win, GLFW_KEY_A) == GLFW_PRESS) glm_vec3_sub(p->cam->pos, right, p->cam->pos);
    if (glfwGetKey(p->win, GLFW_KEY_D) == GLFW_PRESS) glm_vec3_add(p->cam->pos, right, p->cam->pos);

    if (glfwGetKey(p->win, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) p->cam->pos[1] -= move;
    if (glfwGetKey(p->win, GLFW_KEY_SPACE) == GLFW_PRESS) p->cam->pos[1] += move;
}


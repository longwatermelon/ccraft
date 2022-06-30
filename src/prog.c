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

    p->player = player_alloc();

    p->ri = ri_alloc();
    ri_add_shader(p->ri, "shaders/basic_v.glsl", "shaders/basic_f.glsl");
    ri_add_shader(p->ri, "shaders/skybox_v.glsl", "shaders/skybox_f.glsl");

    p->ri->cam = p->player->cam;

    p->skybox = skybox_alloc("res/skybox/");

    return p;
}


void prog_free(struct Prog *p)
{
    player_free(p->player);
    skybox_free(p->skybox);

    ri_free(p->ri);
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

        cam_rot(p->player->cam, (vec3){ 0.f, -(my - prev_my) / 100.f, -(mx - prev_mx) / 100.f });
        prev_mx = mx;
        prev_my = my;

        prog_events(p);

        float fov = 45.f;

        if (p->player->vel[1] < -.2f)
            fov = 45.f + ((-p->player->vel[1] - .2f) * 50.f);

        glm_perspective(glm_rad(fmin(fov, 150.f)), 800.f / 600.f, .1f, 1000.f, p->ri->proj);

        player_update(p->player, w);

        world_gen_chunks(w, p->player->cam->pos);

        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ri_use_shader(p->ri, SHADER_SKYBOX);
        skybox_render(p->skybox, p->ri);

        ri_use_shader(p->ri, SHADER_BASIC);

        player_set_props(p->player, p->ri->shader);
        cam_view_mat(p->player->cam, p->ri->view);

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
    glm_vec3_copy(p->player->cam->rot, angle);
    angle[1] = 0.f;

    vec4 quat;
    util_quat_from_rot(angle, quat);

    vec3 front = { 1.f, 0.f, 0.f };
    vec3 right = { 0.f, 0.f, 1.f };
    glm_quat_rotatev(quat, front, front);
    glm_quat_rotatev(quat, right, right);

    glm_vec3_scale(front, move, front);
    glm_vec3_scale(right, move, right);

    vec3 back, left;
    glm_vec3_negate_to(front, back);
    glm_vec3_negate_to(right, left);

    bool w = glfwGetKey(p->win, GLFW_KEY_W) == GLFW_PRESS;
    bool a = glfwGetKey(p->win, GLFW_KEY_A) == GLFW_PRESS;
    bool s = glfwGetKey(p->win, GLFW_KEY_S) == GLFW_PRESS;
    bool d = glfwGetKey(p->win, GLFW_KEY_D) == GLFW_PRESS;

    if (w) glm_vec3_add(front, p->player->vel, p->player->vel);
    if (a) glm_vec3_add(left, p->player->vel, p->player->vel);
    if (s) glm_vec3_add(back, p->player->vel, p->player->vel);
    if (d) glm_vec3_add(right, p->player->vel, p->player->vel);

    float y = p->player->vel[1];
    p->player->vel[1] = 0.f;
    glm_vec3_normalize(p->player->vel);
    glm_vec3_scale(p->player->vel, move, p->player->vel);
    p->player->vel[1] = y;

    if (!w && !a && !s && !d)
    {
        p->player->vel[0] = 0.f;
        p->player->vel[2] = 0.f;
    }

    if (glfwGetKey(p->win, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        if (p->player->vel[1] == 0.f)
            p->player->vel[1] = .2f;
    }

/*     if (glfwGetKey(p->win, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) p->cam->pos[1] -= move; */
/*     if (glfwGetKey(p->win, GLFW_KEY_SPACE) == GLFW_PRESS) p->cam->pos[1] += move; */
}


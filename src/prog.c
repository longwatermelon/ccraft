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

    p->world = world_alloc();
    p->player = player_alloc();

    p->ri = ri_alloc();
    ri_add_shader(p->ri, "shaders/basic_v.glsl", "shaders/basic_f.glsl");
    ri_add_shader(p->ri, "shaders/skybox_v.glsl", "shaders/skybox_f.glsl");
    ri_add_shader(p->ri, "shaders/color_v.glsl", "shaders/color_f.glsl");

    p->ri->cam = p->player->cam;

    p->skybox = skybox_alloc("res/skybox/");

    glGenVertexArrays(1, &p->crosshair_vao);
    glBindVertexArray(p->crosshair_vao);

    glGenBuffers(1, &p->crosshair_vb);
    glBindBuffer(GL_ARRAY_BUFFER, p->crosshair_vb);

    float verts[] = {
        0.f, .02f * (4.f / 3.f), 0.f, 0.f, 0.f,
        0.f, -.02f * (4.f / 3.f), 0.f, 0.f, 0.f,
        -.02f, 0.f, 0.f, 0.f, 0.f,
        .02f, 0.f, 0.f, 0.f, 0.f
    };

    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return p;
}


void prog_free(struct Prog *p)
{
    glDeleteVertexArrays(1, &p->crosshair_vao);
    glDeleteBuffers(1, &p->crosshair_vb);

    player_free(p->player);
    skybox_free(p->skybox);
    world_free(p->world);

    ri_free(p->ri);
    free(p);
}


void prog_mainloop(struct Prog *p)
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glfwSetCursorPos(p->win, 400.f, 300.f);
    glfwSetInputMode(p->win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    double prev_mx, prev_my;
    glfwGetCursorPos(p->win, &prev_mx, &prev_my);

    /* float prev = glfwGetTime(); */

    while (!glfwWindowShouldClose(p->win))
    {
        /* printf("%f\n", 1.f / (glfwGetTime() - prev)); */
        /* prev = glfwGetTime(); */

        // Camera movement
        double mx, my;
        glfwGetCursorPos(p->win, &mx, &my);

        cam_rot(p->player->cam, (vec3){ 0.f, -(my - prev_my) / 100.f, -(mx - prev_mx) / 100.f });
        prev_mx = mx;
        prev_my = my;

        util_restrict_vangle(p->player->cam->rot, p->player->cam->rot);

        // Restrict y
        float cam_rot_y = p->player->cam->rot[1];

        if (cam_rot_y < 3.f * M_PI / 2.f && cam_rot_y > M_PI / 2.f)
        {
            if (cam_rot_y < M_PI) p->player->cam->rot[1] = M_PI / 2.f;
            else p->player->cam->rot[1] = 3.f * M_PI / 2.f;
        }

        prog_update(p);

        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        prog_render(p);

        glfwSwapBuffers(p->win);
        glfwPollEvents();
    }
}


void prog_update(struct Prog *p)
{
    prog_events(p);

    if (p->player->vel[1] < -.2f)
        p->ri->fov = 45.f + ((-p->player->vel[1] - .2f) * 50.f);
    else
        p->ri->fov += (45.f - p->ri->fov) / 5.f;

    glm_perspective(glm_rad(fmin(p->ri->fov, 150.f)), 800.f / 600.f, .1f, 1000.f, p->ri->proj);

    player_update(p->player, p->world);

    world_gen_chunks(p->world, p->player->cam->pos);
}


void prog_render(struct Prog *p)
{
    // Skybox
    ri_use_shader(p->ri, SHADER_SKYBOX);
    skybox_render(p->skybox, p->ri);

    // Scene
    ri_use_shader(p->ri, SHADER_BASIC);

    player_set_props(p->player, p->ri->shader);
    player_view(p->player, p->ri->view);

    // World
    struct Chunk *c;
    ivec3 coords;
    int type;
    float dist = world_cast_ray(p->world, p->player->cam, &c, coords, &type);

    if (dist > PLAYER_REACH)
        coords[0] = -1; // Don't highlight any block

    world_render(p->world, p->ri, coords);

    // Crosshair
    ri_use_shader(p->ri, SHADER_COLOR);
    glBindVertexArray(p->crosshair_vao);
    glDrawArrays(GL_LINES, 0, 4);
    glBindVertexArray(0);
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

    static float last_lmb = 0.f;
    static float last_rmb = 0.f;

    if (glfwGetMouseButton(p->win, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && glfwGetTime() - last_lmb > .3f)
    {
        last_lmb = glfwGetTime();
        player_destroy_block(p->player, p->world);
    }

    if (glfwGetMouseButton(p->win, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS && glfwGetTime() - last_rmb > .3f)
    {
        last_rmb = glfwGetTime();
        player_place_block(p->player, p->world);
    }

    if (glfwGetMouseButton(p->win, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
    {
        struct Chunk *c;
        ivec3 coords;
        int type;
        if (world_cast_ray(p->world, p->player->cam, &c, coords, &type) != INFINITY)
            p->player->block = c->grid[coords[0]][coords[1]][coords[2]];
    }

/*     if (glfwGetKey(p->win, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) p->cam->pos[1] -= move; */
/*     if (glfwGetKey(p->win, GLFW_KEY_SPACE) == GLFW_PRESS) p->cam->pos[1] += move; */
}


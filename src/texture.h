#ifndef TEXTURE_H
#define TEXTURE_H

#include "shader.h"
#include <limits.h>

enum { TEXTURETYPE_DIFFUSE, TEXTURETYPE_SPECULAR };

struct Texture
{
    int type;
    unsigned int id;

    char path[PATH_MAX];
};

struct Texture *tex_alloc(const char *path, int type);
void tex_free(struct Texture *t);

void tex_bind(struct Texture *t, unsigned int slot);

#endif


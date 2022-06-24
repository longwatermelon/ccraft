#ifndef UTIL_H
#define UTIL_H

#include <cglm/cglm.h>

#define ARR_APPEND(arr, n, arr2, n2, type) {\
    arr = realloc(arr, sizeof(type) * (n + n2)); \
    memcpy(&arr[n], arr2, sizeof(type) * n2); \
    n += n2; \
}

char *util_read_file(const char *path);

void util_quat_from_rot(vec3 rot, vec4 dest);
void util_eul2quat(vec3 rot, vec4 dest);

#endif


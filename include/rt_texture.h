#ifndef RT_TEXTURE_H
#define RT_TEXTURE_H

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <float.h>
#include <math.h>
#include "nvec.h"
#include "mathc.h"


struct rt_texture{
    float *data;
    size_t width, height;
    size_t stride;
    
    int (*interp)(const struct rt_texture *src, const float pos[2], float *dst);
};

struct rt_texture *rt_texture_init(struct rt_texture *dst, size_t width, size_t height, size_t stride);
struct rt_texture *rt_texture_load(struct rt_texture *dst, const char *path);
void rt_texture_free(struct rt_texture *dst);

int rt_texture_write(struct rt_texture *src, const char *path);

float *rt_texture_at(const struct rt_texture *src, size_t x, size_t y);

int rt_texture_interp_default(const struct rt_texture *src, const float pos[2], float *dst);

static inline int rt_texture_interp(const struct rt_texture *src, const float pos[2], float *dst){
    if(src->interp != NULL)
        return src->interp(src, pos, dst);
    return 0;
}

#endif //RT_TEXTURE_H

#ifndef RT_TRACE_H
#define RT_TRACE_H

#include "rt_geo.h"
#include "rt_ray.h"
#include "nvec.h"
#include "util.h"
#include <float.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "rt_texture.h"

struct rt_renderer{
    struct rt_tracer *tracer;
    struct rt_texture *texture;
    double focal_length;
    struct vec2 img_offset;
    size_t depth;
};

struct rt_tracer{
    struct rt_geo *geo;
    const struct rt_texture *textures;
    size_t dst_len;
    size_t split_n;

    int (*shader_geo_cb)(struct rt_tracer *src, const float *vert, float dst[3]);
    int (*shader_ray_cb)(struct rt_tracer *src, const float *point, const struct vec3 *rand_s, struct rt_ray *dst);
    //int (*shader_pix_cb)(struct rt_tracer *src, const float *point, size_t point_len, float *prev, size_t prev_len, float *dst);
    int (*shader_comp_cb)(struct rt_tracer *src, const float *point, const float *src_arr, float *dst);
    int (*shader_attr_cb)(struct rt_tracer *src, float *dst, const float *src_varr[3], size_t len, const struct vec3 *src_uvt, size_t attr_idx);
};

struct rt_tracer *rt_tracer_init(struct rt_tracer *dst, size_t dst_len, size_t split_n);

int rt_tracer_trace(struct rt_tracer *tr, struct rt_ray *ray, struct vec3 *dst_uvt, size_t *dst_idx);
int rt_tracer_trace_rec(struct rt_tracer *tr, struct rt_ray *ray, float *dst, int lvl);
int rt_tracer_calc_attr(struct rt_tracer *tr, float *dst, const struct vec3 *src_uvt, size_t src_idx);

int rt_renderer_render(struct rt_renderer *dst);



#endif //RT_TRACE_H

#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "rt_trace.h"

int shader_geo_cb(struct rt_tracer *src, const float *vert, float dst[3]);
int shader_ray_cb(struct rt_tracer *src, const float *point, const struct vec3 *rand_s, struct rt_ray *dst);
int shader_comp_cb(struct rt_tracer *src, const float *point, const float *src_arr, float *dst);
int shader_attr_cb(struct rt_tracer *src, float *dst, const float *src_varr[3], size_t len, const struct vec3 *src_uvt, size_t attr_idx);

int main(){
    struct rt_tracer tr;
    struct rt_geo geo;
    struct rt_texture output;
    rt_texture_init(&output, 200, 200, 3);
    struct rt_texture tex_in[1];
    rt_texture_load(&tex_in[0], "res/img/test.png");
    struct rt_renderer rd = {
        .tracer = &tr,
        .texture = &output,
        .focal_length = 0.01,
        .img_offset = svec2(0, 0),
        .depth = 1,
    };


    geo.vert_buf = (float []){
        1.0, 0.0, 0.0,  0.0, 0.0,
        0.0, 1.0, 0.0,  0.0, 200.0, 
        0.0, 0.0, 1.0,  200.0, 0.0,
        -1.0, 0.0, 0.0,  1.0, 0.0,
        0.0, -1.0, 0.0,  1.0, 0.0,
    };
    geo.vert_buf_len = 30;
    geo.stride = 5;

    geo.idx_buf = (int []){
        0, 1, 2,
        0, 4, 2,
        1, 3, 2,
        3, 4, 2,
    };
    geo.idx_buf_len = 3;

    geo.attr_buf = (struct rt_attr[]){
        (struct rt_attr){.offset = 0, .len = 3},
        (struct rt_attr){.offset = 3, .len = 2},
    };
    geo.attr_buf_len = 2;

    tr.geo = &geo;

    tr.dst_len = 3;
    tr.split_n = 10;

    tr.shader_attr_cb = shader_attr_cb;
    tr.shader_comp_cb = shader_comp_cb;
    tr.shader_geo_cb = shader_geo_cb;
    tr.shader_ray_cb = shader_ray_cb;

    tr.textures = tex_in;

    float dst[3];
    struct rt_ray ray = {.orig = svec3(0, 0, 0), .dir = svec3(0.001, 0.001, 1)};

    //rt_tracer_trace_rec(&tr, &ray, dst, 1);

    //printf("%f, %f, %f\n", dst[0], dst[1], dst[2]);
    rt_renderer_render(&rd);

    rd.texture->data[0] = 1.0;
    rd.texture->data[1] = 0.0;
    rd.texture->data[2] = 0.0;
    rt_texture_write(rd.texture, "test.png");

    rt_texture_free(rd.texture);

    return 0;
}


int shader_geo_cb(struct rt_tracer *src, const float *vert, float dst[3]){
    vec3_assign(dst, (float *)vert);
    return 1;
}
int shader_ray_cb(struct rt_tracer *src, const float *point, const struct vec3 *rand_s, struct rt_ray *dst){
    dst->orig = svec3(point[0], point[1], point[2]);
    dst->dir = *rand_s;
    return 1;
}
int shader_comp_cb(struct rt_tracer *src, const float *point, const float *src_arr, float *dst){
    rt_texture_interp(&src->textures[0], &point[3], dst);
    return 0;
    dst[0] = rt_texture_at(&src->textures[0], point[3], point[4])[0];
    dst[1] = rt_texture_at(&src->textures[0], point[3], point[4])[1];
    dst[2] = rt_texture_at(&src->textures[0], point[3], point[4])[2];
    return 0;
    for(size_t i = 0;i < src->dst_len;i++){
        dst[i] = 0;
        for(size_t j = 0;j < src->split_n;j++){
            dst[i] += src_arr[src->dst_len * j + i] * 1/(src->split_n + 1);
        }
        dst[i] += point[3 + i] * 1/(src->split_n + 1);
    }
    return 1;
}
int shader_attr_cb(struct rt_tracer *src, float *dst, const float *src_varr[3], size_t len, const struct vec3 *src_uvt, size_t attr_idx){
    if(attr_idx == -1){
        //printf("%f, %f\n", src_uvt->x, src_uvt->y);
        dst[0] = 1.0;
        dst[1] = 0.0;
        dst[2] = 0.0;
    }
    else{
        float tmp[len];
        float vert12_attr[len];
        float vert13_attr[len];

        nvec_sub(vert12_attr, src_varr[1], src_varr[0], len);
        nvec_sub(vert13_attr, src_varr[2], src_varr[0], len);

        nvec_copy(dst, src_varr[0], len);

        nvec_scale(tmp, vert12_attr, src_uvt->x, len);
        nvec_add(dst, dst, tmp, len);

        nvec_scale(tmp, vert13_attr, src_uvt->y, len);
        nvec_add(dst, dst, tmp, len);

        if(attr_idx == 1){
            //printf("%f, %f\n", dst[0], dst[1]);
        }

        return 1;
    }
}

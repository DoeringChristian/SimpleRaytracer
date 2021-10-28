#include "rt_texture.h"

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif //STB_IMAGE_IMPLEMENTATION
#ifndef STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#endif //STB_IMAGE_WRITE_IMPLEMENTATION

struct rt_texture *rt_texture_init(struct rt_texture *dst, size_t width, size_t height, size_t stride){
    dst->width = width;
    dst->height = height;
    dst->stride = stride;
    dst->data = malloc(sizeof(float) * width * height * stride);
    dst->interp = rt_texture_interp_default;
    return dst;
}
struct rt_texture *rt_texture_load(struct rt_texture *dst, const char *path){
    int width, height, channels;
    uint8_t *img = stbi_load(path, &width, &height, &channels, 0);
    if(img == NULL)
        return NULL;

    dst->interp = rt_texture_interp_default;
    dst->width = (size_t)width;
    dst->height = (size_t)height;
    dst->stride = (size_t)channels;
    dst->data = malloc(sizeof(float) * dst->width *dst->height * dst->stride);
    for(size_t i = 0;i < dst->width * dst->height * dst->stride;i++){
        dst->data[i] = (float)img[i] / 255;
    }

    stbi_image_free(img);
    return dst;
}
void rt_texture_free(struct rt_texture *dst){
    free(dst->data);
    dst->width = 0;
    dst->height = 0;
    dst->stride = 0;
}

float *rt_texture_at(const struct rt_texture *src, size_t x, size_t y){
    return &src->data[src->stride * (x + y * src->width)];
}

int rt_texture_interp_default(const struct rt_texture *src, const float pos[2], float *dst){
    struct vec2 p00 = svec2(floor(pos[0]), floor(pos[1]));
    struct vec2 p01 = svec2(floor(pos[0]), ceil(pos[1]));
    struct vec2 p10 = svec2(ceil(pos[0]), floor(pos[1]));
    struct vec2 p11 = svec2(ceil(pos[0]), ceil(pos[1]));
    struct vec2 spos = svec2(pos[0], pos[1]);

#if 0
    float dist_p00 = svec2_length(svec2_subtract(spos, p00));
    float dist_p01 = svec2_length(svec2_subtract(spos, p01));
    float dist_p10 = svec2_length(svec2_subtract(spos, p10));
    float dist_p11 = svec2_length(svec2_subtract(spos, p11));
#endif
    float d = (ceil(pos[0]) - floor(pos[0])) * (ceil(pos[1]) - floor(pos[1]));

    float w00 = (ceil(pos[0]) - pos[0]) * (ceil(pos[1]) - pos[1]) / d;
    float w01 = (ceil(pos[0]) - pos[0]) * (pos[1] - floor(pos[1])) / d;
    float w10 = (pos[0] - floor(pos[0])) * (ceil(pos[1]) - pos[1]) / d;
    float w11 = (pos[0] - floor(pos[0])) * (pos[1] - floor(pos[1])) / d;

    float tmp[src->stride];
    nvec_scale(tmp, rt_texture_at(src, p00.x, p00.y), w00, src->stride);
    nvec_add(dst, dst, tmp, src->stride);
    nvec_scale(tmp, rt_texture_at(src, p01.x, p01.y), w01, src->stride);
    nvec_add(dst, dst, tmp, src->stride);
    nvec_scale(tmp, rt_texture_at(src, p10.x, p10.y), w10, src->stride);
    nvec_add(dst, dst, tmp, src->stride);
    nvec_scale(tmp, rt_texture_at(src, p11.x, p11.y), w11, src->stride);
    nvec_add(dst, dst, tmp, src->stride);
    return 1;
}

int rt_texture_write(struct rt_texture *src, const char *path){
    uint8_t *img = malloc(src->width * src->height * src->stride);

    for(size_t i = 0;i < src->width * src->height * src->stride;i++)
        img[i] = 255 * src->data[i];

    stbi_write_png(path, src->width, src->height, src->stride, img, src->width * src->stride);

    free(img);
    return 1;
}

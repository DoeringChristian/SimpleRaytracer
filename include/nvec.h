#ifndef NVEC_H
#define NVEC_H

#include <stdint.h>
#include <stddef.h>

static inline float *nvec_zero(float *dst, size_t len){
    for(size_t i = 0;i < len;i++)
        dst[i] = 0;
    return dst;
}

static inline float *nvec_copy(float *dst, const float *src, size_t len){
    for(size_t i = 0;i < len;i++)
        dst[i] = src[i];
    return dst;
}

static inline float *nvec_scale(float *dst, const float *src, float s, size_t len){
    for(size_t i = 0;i < len;i++)
        dst[i] = src[i] * s;
    return dst;
}
static inline float nvec_mult(const float *src1, const float *src2, size_t len){
    float dst = 0;
    for(size_t i = 0;i < len;i++)
        dst += src1[i] * src2[i];
    return dst;
}
static inline float *nvec_add(float *dst, const float *src1, float *src2, size_t len){
    for(size_t i = 0;i < len;i++)
        dst[i] = src1[i] + src2[i];
    return dst;
}
static inline float *nvec_sub(float *dst, const float *src1, const float *src2, size_t len){
    for(size_t i = 0;i < len;i++)
        dst[i] = src1[i] - src2[i];
    return dst;
}

static inline float *nvec_clamp(float *dst, const float *src, float clamp, size_t len){
    for(size_t i = 0;i < len;i++)
        if(src[i] >= clamp)
            dst[i] = clamp;
    return dst;
}

#endif //NVEC_H

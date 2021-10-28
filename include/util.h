#ifndef UTIL_H
#define UTIL_H

#include <stdlib.h>
#include "mathc.h"

#define RAND_SPHERE_MAX 100

static inline float rand_f(){
    return ((float)(rand())) / (float)RAND_MAX;
}

static inline struct vec3 rand_2sphere(){
    struct vec3 rand;
    for(size_t i = 0;svec3_length(rand) > 1 && i < RAND_SPHERE_MAX; i++){
        rand = svec3(rand_f(), rand_f(), rand_f());
    }
    psvec3_multiply_f(&rand, &rand, 1 / psvec3_length(&rand));
    return rand;
}

static inline struct vec3 svec3_blend_rgb(struct vec3 src1, struct vec3 src2, float ratio){
    struct vec3 dst = svec3(0, 0, 0);
    dst.x = (src1.x * (1-ratio)) + (src2.x * ratio);
    dst.y = (src1.y * (1-ratio)) + (src2.y * ratio);
    dst.y = (src1.z * (1-ratio)) + (src2.z * ratio);

    return dst;
}

static inline struct vec4 svec4_blend_rgba(struct vec4 src1, struct vec4 src2){

}

#endif //UTIL_H

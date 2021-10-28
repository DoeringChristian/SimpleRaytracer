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

#endif //UTIL_H

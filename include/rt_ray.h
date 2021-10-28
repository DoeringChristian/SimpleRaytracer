#ifndef RT_RAY_H
#define RT_RAY_H

#include <stdint.h>
#include <stddef.h>
#include "mathc.h"

struct rt_ray{
    struct vec3 orig, dir;
};

#endif //RT_RAY_H

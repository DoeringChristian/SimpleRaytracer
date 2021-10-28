#ifndef RT_GEO_H
#define RT_GEO_H

#include <stdint.h>
#include <stddef.h>
#include "mathc.h"

struct rt_attr{
    size_t offset, len;
};

struct rt_geo{
    size_t vert_buf_len;
    const float *vert_buf;
    size_t idx_buf_len;
    const int *idx_buf;
    size_t attr_buf_len;
    const struct rt_attr *attr_buf;
    size_t stride;
    size_t pos_attr;
};


#endif //RT_GEO_H

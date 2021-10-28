#include "rt_trace.h"

int rt_tracer_trace(struct rt_tracer *tr, struct rt_ray *ray, struct vec3 *dst_uvt, size_t *dst_idx){
    *dst_idx = 0;
    *dst_uvt = svec3(0, 0, FLT_MAX);
    for(size_t i = 0;i < tr->geo->idx_buf_len/3;i++){
        // exclude triangles that are in the wrong hemisphere
        //struct vec3 v1, v2, v3;
        float v1[3], v2[3], v3[3], v12[3], v13[3];

        size_t idx0, idx1, idx2;
        idx0 = tr->geo->idx_buf[i * 3 + 0];
        idx1 = tr->geo->idx_buf[i * 3 + 1];
        idx2 = tr->geo->idx_buf[i * 3 + 2];

        tr->shader_geo_cb(tr, &tr->geo->vert_buf[tr->geo->stride * idx0], v1);
        tr->shader_geo_cb(tr, &tr->geo->vert_buf[tr->geo->stride * idx1], v2);
        tr->shader_geo_cb(tr, &tr->geo->vert_buf[tr->geo->stride * idx2], v3);
        vec3_subtract(v12, v2, v1);
        vec3_subtract(v13, v3, v1);
#if 0
        printf("%f, %f, %f\n", v1[0], v1[1], v1[2]);
        printf("%f, %f, %f\n", v2[0], v2[1], v2[2]);
        printf("%f, %f, %f\n", v3[0], v3[1], v3[2]);
        printf("%f, %f, %f\n", ray->dir.x, ray->dir.y, ray->dir.z);
#endif

        struct mat3 m0 = smat3(
                v12[0], v13[0], -ray->dir.x,
                v12[1], v13[1], -ray->dir.y,
                v12[2], v13[2], -ray->dir.z
                );
        m0 = smat3_transpose(m0);

        struct mat3 m0_inv = smat3(
                    m0.m22 * m0.m33 - m0.m23 * m0.m32, m0.m13 * m0.m32 - m0.m12 * m0.m33, m0.m12 * m0.m23 - m0.m13 * m0.m22,
                    m0.m23 * m0.m31 - m0.m12 * m0.m33, m0.m11 * m0.m33 - m0.m13 * m0.m31, m0.m13 * m0.m21 - m0.m11 * m0.m23,
                    m0.m21 * m0.m32 - m0.m22 * m0.m31, m0.m12 * m0.m31 - m0.m11 * m0.m32, m0.m11 * m0.m22 - m0.m12 * m0.m21
                 );
        float det = psmat3_determinant(&m0);
        // det == 0 => matrix not invertible
        if(det == 0)
            continue;
        m0_inv = smat3_multiply_f(m0_inv, 1/ det);

        struct vec3 voff = svec3_assign(ray->orig);
        struct vec3 sv1 = svec3(v1[0], v1[1], v1[2]);
        voff = svec3_subtract(voff, sv1);

#if 1
        struct mat3 voff_mat = smat3(
                voff.x, 0, 0,
                voff.y, 0, 0,
                voff.z, 0, 0
                );
#else
        struct mat3 voff_mat = smat3(
                voff.x, voff.y, voff.z,
                0, 0, 0,
                0, 0, 0
                );
#endif
        struct mat3 uvt_mat;
        struct vec3 uvt;

        psmat3_multiply(&uvt_mat, &m0_inv, &voff_mat);
        uvt = svec3(uvt_mat.m11, uvt_mat.m21, uvt_mat.m31);

#if 0
        if(ray->dir.x > 0 && ray->dir.y > 0)
            printf("tri: (%f, %f, %f) (%f, %f, %f) (%f, %f, %f) ray: (%f, %f, %f) (%f, %f, %f) uvt: (%f, %f, %f)\n",
                v1[0], v1[1], v1[2],
                v2[0], v2[1], v2[2],
                v3[0], v3[1], v3[2],
                ray->orig.x, ray->orig.y, ray->orig.z,
                ray->dir.x, ray->dir.y, ray->dir.z,
                uvt.x, uvt.y, uvt.z);
#endif

#if 0
        if(uvt.x > 0 && uvt.y > 0 && uvt.x + uvt.y < 1 && ray->dir.x > 0 && ray->dir.y > 0)
            printf("%f, %f, %f\n", uvt.x, uvt.y, uvt.z);
#endif

        if(uvt.x < 0 || uvt.y < 0 || uvt.x + uvt.y > 1 || uvt.z < 0 || uvt.z >= dst_uvt->z)
            continue;
        psvec3_assign(dst_uvt, &uvt);
#if 0
        psvec3(&dst_cords[0], v1[0], v1[1], v1[2]);
        psvec3(&dst_cords[1], v2[0], v2[1], v2[2]);
        psvec3(&dst_cords[2], v3[0], v3[1], v3[2]);
#endif
    }

    if(dst_uvt->z == FLT_MAX)
        return 0;
    return 1;
}

int rt_tracer_trace_rec(struct rt_tracer *tr, struct rt_ray *ray, float *dst, int lvl){
    if(lvl <= 0)
        return 0;
    struct vec3 uvt;
    size_t idx;
    float point[tr->geo->stride];
    if(!rt_tracer_trace(tr, ray, &uvt, &idx)){
        for(size_t i = 0;i < tr->dst_len;i++)
            dst[i] = 0.0;
        return 0;
    }

    //printf("%f, %f, %f\n", uvt.x, uvt.y, uvt.z);
    //printf("%f, %f, %f\n", ray->dir.x, ray->dir.y, ray->dir.z);

    rt_tracer_calc_attr(tr, point, &uvt, idx);

    struct vec3 rand;
    time_t t;
    struct rt_ray ray_next;

    float *dst_arr = malloc(sizeof(float) * tr->dst_len * tr->split_n);

    for(size_t i = 0;i < tr->split_n;i++){
        srand((unsigned) time(&t));

        rand = rand_2sphere();

        tr->shader_ray_cb(tr, point, &rand, &ray_next);

        if(rt_tracer_trace_rec(tr, &ray_next, &dst_arr[i * tr->dst_len], lvl-1) == 0){
            nvec_zero(&dst_arr[tr->dst_len * i], tr->dst_len);
        } 
    }

    // floats that are nan have to be reset to 0 to not poision the rest.
#if 1
    for(size_t i = 0;i < tr->dst_len * tr->split_n;i++)
        if(isnan(dst_arr[i]))
            dst_arr[i] = 0;
#endif

    tr->shader_comp_cb(tr, point, dst_arr, dst);

    free(dst_arr);

    return 1;
}

int rt_tracer_calc_attr(struct rt_tracer *tr, float *dst, const struct vec3 *src_uvt, size_t src_idx){

    // lineary scale all attributest acording to the uv of the triangle that has been hit.

    for(size_t i = 0;i < tr->geo->attr_buf_len;i++){
        struct rt_attr attr = tr->geo->attr_buf[i];
        size_t idx1, idx2, idx3;

        idx1 = tr->geo->idx_buf[src_idx + 0];
        idx2 = tr->geo->idx_buf[src_idx + 1];
        idx3 = tr->geo->idx_buf[src_idx + 2];


        const float *vert1_attr = &tr->geo->vert_buf[idx1 * tr->geo->stride + attr.offset];
        const float *vert2_attr = &tr->geo->vert_buf[idx2 * tr->geo->stride + attr.offset];
        const float *vert3_attr = &tr->geo->vert_buf[idx3 * tr->geo->stride + attr.offset];

        const float *attr_arr[3] = {vert1_attr, vert2_attr, vert3_attr};

        tr->shader_attr_cb(tr, &dst[attr.offset], attr_arr, attr.len, src_uvt, i);

#if 0
        struct rt_attr attr = tr->geo.attr_buf[i];
        size_t idx1, idx2, idx3;

        idx1 = tr->geo.idx_buf[src_idx + 0];
        idx2 = tr->geo.idx_buf[src_idx + 1];
        idx3 = tr->geo.idx_buf[src_idx + 2];

        //tr->geo.vert_buf[idx + 0 + attr.offset]; 
        const float *vert1_attr = &tr->geo.vert_buf[idx1 * tr->geo.stride + attr.offset];
        const float *vert2_attr = &tr->geo.vert_buf[idx2 * tr->geo.stride + attr.offset];
        const float *vert3_attr = &tr->geo.vert_buf[idx3 * tr->geo.stride + attr.offset];

        // calc
        float tmp[attr.len];
        float vert12_attr[attr.len];
        float vert13_attr[attr.len];

        nvec_sub(vert12_attr, vert2_attr, vert1_attr, attr.len);
        nvec_sub(vert13_attr, vert3_attr, vert1_attr, attr.len);

        nvec_copy(&dst[attr.offset], vert1_attr, attr.len);

        nvec_scale(tmp, vert12_attr, src_uvt->x, attr.len);
        nvec_add(&dst[attr.offset], &dst[attr.offset], tmp, attr.len);

        nvec_scale(tmp, vert13_attr, src_uvt->y, attr.len);
        nvec_add(&dst[attr.offset], &dst[attr.offset], tmp, attr.len);
#endif
    }
    return 1;
}

int rt_renderer_render(struct rt_renderer *dst){
    for(size_t i = 0;i < dst->texture->width;i++){
        for(size_t j = 0;j < dst->texture->height;j++){
            struct vec3 dir = svec3(0, 0, 1);
            dir = svec3_add(dir, svec3(dst->img_offset.x, dst->img_offset.y, 0));
            dir = svec3_add(dir, svec3(
                        (i - (float)dst->texture->width / 2) * dst->focal_length,
                        (j - (float)dst->texture->height / 2) * dst->focal_length, 
                        0));
            struct rt_ray ray = {
                .orig = svec3(0, 0, 0),
                .dir = dir,
            };
            float output[dst->tracer->dst_len];
            rt_tracer_trace_rec(dst->tracer, &ray, output, dst->depth);
            nvec_copy(rt_texture_at(dst->texture, i, j), output, dst->texture->stride);
        }
    }
    return 1;
}

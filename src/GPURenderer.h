
#ifndef GPURENDERED_H
#define GPURENDERED_H

#include "ray.h"


namespace GPURenderer{

    __device__ bool hit_sphere(const vec3& center, float radius, const ray& r) {
        vec3 oc = r.origin() - center;
        float a = dot(r.direction(), r.direction());
        float b = 2.0f * dot(oc, r.direction());
        float c = dot(oc, oc) - radius*radius;
        float discriminant = b*b - 4.0f*a*c;
        return (discriminant > 0.0f);
    }

    __device__ vec3 color(const ray& r) {
        if (hit_sphere(vec3(0,0,-1), 0.5, r))
            return vec3(1,0,0);
        vec3 unit_direction = glm::normalize(r.direction());
        float t = 0.5f*(unit_direction.y + 1.0f);
        return (1.0f-t)*vec3(1.0, 1.0, 1.0) + t*vec3(0.5, 0.7, 1.0);
    }

    __global__ void render(vec3 *fb, int max_x, int max_y,
                        vec3 lower_left_corner, vec3 horizontal, vec3 vertical, vec3 origin) {
        int i = threadIdx.x + blockIdx.x * blockDim.x;
        int j = threadIdx.y + blockIdx.y * blockDim.y;
        if((i >= max_x) || (j >= max_y)) return;
        int pixel_index = j*max_x + i;
        float u = float(i) / float(max_x);
        float v = float(j) / float(max_y);
        ray r(origin, lower_left_corner + u*horizontal + v*vertical);
        fb[pixel_index] = color(r);
    }

}
#endif
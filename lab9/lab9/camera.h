#ifndef _CSCI441_CAMERA_H_
#define _CSCI441_CAMERA_H_

#include <glm/glm.hpp>

#include "ray.h"

struct Camera {
    glm::vec3 pos;

    float viewport_dist;
    glm::vec2 viewport_min;
    glm::vec2 viewport_max;

    glm::vec3 u;
    glm::vec3 v;
    glm::vec3 w;

    glm::vec3 background;

    Camera(const glm::vec3& eye,
            const glm::vec3& target,
            const glm::vec3& up,
            const glm::vec2& viewport_min,
            const glm::vec2& viewport_max,
            float viewport_dist,
            const glm::vec3& background)
        : pos(eye),
        viewport_min(viewport_min),
        viewport_max(viewport_max),
        viewport_dist(viewport_dist),
        background(background) {

        w = glm::normalize(eye-target);
        u = glm::normalize(glm::cross(w,up));
        v = glm::cross(u, w);
    }

    Ray make_ray(int nx, int ny, int i, int j) const {
        float l = viewport_min.x;
        float r = viewport_max.x;
        float b = viewport_min.y;
        float t = viewport_max.y;

        float us = l + (r-l)*(i+.5) / nx;
        float vs = b + (t-b)*(j+.5) / ny;

        Ray ray;
        ray.origin = pos;
        ray.direction = -viewport_dist*w + us*u + vs*v;
        ray.direction = glm::normalize(ray.direction);
        return ray;
    }
};

#endif

#ifndef _CSCI441_RAY_H_
#define _CSCI441_RAY_H_

#include <glm/glm.hpp>

struct Ray {
    glm::vec3 origin;
    glm::vec3 direction;

    glm::vec3 eval(float t) const {
        return origin + t*direction;
    }
};

#endif

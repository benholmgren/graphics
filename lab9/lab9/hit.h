#ifndef _CSCI441_HIT_H_
#define _CSCI441_HIT_H_

#include <assert.h>

#include <glm/glm.hpp>

#include "shape.h"
#include "ray.h"

class Hit {
private:
    const Shape* _shape;
    Ray _ray;

public:
    double _t;
    Hit(const Ray& r, double t=1000) : _shape(NULL), _ray(r), _t(t) {}

    void update(const Shape* s, double new_t) {
        _shape = s;
        _t = new_t;
    }

    double t() const {
        return _t;
    }

    bool is_intersection() const {
        return _shape != NULL;
    }

    glm::vec3 position() const {
        assert(is_intersection());

        return _ray.eval(_t);
    }

    glm::vec3 normal() const {
        assert(is_intersection());
        return _shape->normal(position());
    }

    glm::vec3 color() const {
        assert(is_intersection());
        return _shape->color(position());
    }
};


#endif

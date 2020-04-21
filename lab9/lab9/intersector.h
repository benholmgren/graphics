#ifndef _CSCI441_INTERSECTOR_H_
#define _CSCI441_INTERSECTOR_H_

#include <vector>

#include "ray.h"
#include "shape.h"
#include "world.h"


class Intersector {
public:
    virtual ~Intersector() { }

    virtual Hit find_first_intersection(const World& world, const Ray& ray) = 0;
};


#endif

#ifndef _CSCI441_WORLD_H_
#define _CSCI441_WORLD_H_

#include <assert.h>

#include "shape.h"

class World {
private:
    bool _locked;
    std::vector<const Shape*> _shapes;
    std::vector<Sphere> _spheres;
    std::vector<Triangle> _triangles;

    template <typename InputIter>
    void push_back(InputIter begin, InputIter end) {
        for (InputIter i = begin ; i != end ; ++i) {
            _shapes.push_back(&(*i));
        }
    }

public:

    World() : _locked(false) {}

    bool is_locked() const {
        return _locked;
    }

    void append(const Sphere& s) {
        assert(!is_locked());
        _spheres.push_back(s);
    }

    void append(const Triangle& t) {
        assert(!is_locked());

        _triangles.push_back(t);
    }

    template <typename InputIter>
    void append(InputIter begin, InputIter end) {
        for (InputIter o = begin ; o != end ; ++o) {
            append(*o);
        }
    }

    void append(const std::vector<Triangle> triangles) {
        append(triangles.begin(), triangles.end());
    }

    void lock() {
        assert(!is_locked());

        _locked = true;
        _shapes.clear();
        push_back(_spheres.begin(), _spheres.end());
        push_back(_triangles.begin(), _triangles.end());
    }

    const std::vector<const Shape*>& shapes() const {
        assert(is_locked());
        return _shapes;
    }
};

#endif

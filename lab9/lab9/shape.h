#ifndef _CSCI441_SHAPE_H_
#define _CSCI441_SHAPE_H_

#include <glm/glm.hpp>

class Shape {
private:
    int _id;

    int next_id() {
        static int id_seed = 0;
        return ++id_seed;
    }

public:
    Shape() : _id(next_id()) {}
    Shape(const Shape& s) : _id(s.id()) { }
    virtual ~Shape() {}

    /** @return id of shape */
    int id() const {
        return _id;
    }

    /**
     * @return the default zero value
     */
    double default_zero() const {
        return 0;
    }

    /**
     * @return the default inf value
     */
    double default_inf() const {
        return std::numeric_limits<double>::infinity();
    }

    /**
     * Compute the normal at point p
     *
     * @param p is assumed to be on the surface of the shape
     * @return the normal on the surface at point p
     */
    virtual glm::vec3 normal(const glm::vec3& p) const {
        return glm::vec3(0,0,0);
    }

    /**
     * Compute the color at point p
     *
     * @param p is assumed to be (approx) on the surface of the shape
     * @return the color at point p
     */
    virtual glm::vec3 color(const glm::vec3& p) const {
        return glm::vec3(0,0,0);
    }

    /**
     * Compute the first intersection of the ray and the surface
     * that is larger than the specified "zero" and smaller that the
     * specified "infinity" using the defaults for default_zero()
     * and default_inf()
     *
     * @param r the ray to intersect with the surface.
     * @return the param of the intersection,
     *          or value for inf if there is no intersection
     */
    virtual double intersect(const Ray& r) const {
        return intersect(r, default_zero(), default_inf());
    }

    /**
     * Compute the first intersection of the ray and the surface
     * that is larger than the specified "zero" and smaller that the
     * specified "infinity".
     *
     * @param r the ray to intersect with the surface.
     * @param zero the value for zero
     * @param inf the value for infinity
     * @return the param of the intersection,
     *          or value for inf if there is no intersection
     */
    virtual double intersect(const Ray& r, double zero, double inf) const {
        return inf;
    }
};



struct Sphere : public Shape {
private:
    glm::vec3 _color;
    glm::vec3 _center;
    float _radius;

public:
    Sphere(
        const glm::vec3& center=glm::vec3(0,0,0),
        float radius=0,
        const glm::vec3& color=glm::vec3(0,0,0)
    ) : Shape(), _center(center), _radius(radius), _color(color) { }

    glm::vec3 normal(const glm::vec3& p) const {
        return glm::normalize(p-_center);
    }

    glm::vec3 color(const glm::vec3&) const {
        return _color;
    }

    glm::vec3 center() const { return _center; }

    double intersect(const Ray& r, double zero, double inf) const {
        double t = inf;

        glm::vec3 l = _center - r.origin;

        double t_ca = glm::dot(l, r.direction);

        double d2 = glm::dot(l, l) - t_ca*t_ca;
        double r2 = _radius*_radius;
        if (t_ca >= 0 && d2 <= r2) {
            double t_hc = sqrt(r2 - d2);
            double t0 = t_ca - t_hc;
            double t1 = t_ca + t_hc;

            if (t0 > t1) {
                std::swap(t0, t1);
            }

            if (t0 > zero) {
                t = t0;
            } else if (t1 > zero) {
                t = t1;
            }
        }
        return t;
    }
};


struct Triangle : public Shape {
public:
    glm::vec3 _a, _b, _c, _color;
    Triangle(
        const glm::vec3& a,
        const glm::vec3& b,
        const glm::vec3& c,
        const glm::vec3& color=glm::vec3(0,0,0)
    ) : Shape(), _a(a), _b(b), _c(c), _color(color) {}

    glm::vec3 normal(const glm::vec3&) const {
        return glm::normalize(glm::cross(_b-_a, _c-_a));
    }

    glm::vec3 color(const glm::vec3&) const {
        return _color;
    }

    double intersect(const Ray& r, double zero, double inf) const {
        glm::mat3 A(_b-_a, _c-_a, -r.direction);
        glm::vec3 b = r.origin-_a;

        glm::vec3 x = glm::inverse(A) * b;
        float u = x.x;
        float v = x.y;
        float t = x.z;

        return (v < 0 || 1 < v || u < 0 || 1-v < u || t < zero) ? inf : t;
    }
};


struct Obj {

    static std::vector<Triangle> make_box(const glm::vec3 c, float s, const glm::vec3 color) {
        return {
            Triangle(c+s*glm::vec3(-0.5f, -0.5f, -0.5f), c+s*glm::vec3( 0.5f, -0.5f, -0.5f), c+s*glm::vec3( 0.5f,  0.5f, -0.5f), color),
            Triangle(c+s*glm::vec3( 0.5f,  0.5f, -0.5f), c+s*glm::vec3(-0.5f,  0.5f, -0.5f), c+s*glm::vec3(-0.5f, -0.5f, -0.5f), color),

            Triangle(c+s*glm::vec3(-0.5f, -0.5f,  0.5f), c+s*glm::vec3( 0.5f, -0.5f,  0.5f), c+s*glm::vec3( 0.5f,  0.5f,  0.5f), color),
            Triangle(c+s*glm::vec3( 0.5f,  0.5f,  0.5f), c+s*glm::vec3(-0.5f,  0.5f,  0.5f), c+s*glm::vec3(-0.5f, -0.5f,  0.5f), color),

            Triangle(c+s*glm::vec3(-0.5f,  0.5f,  0.5f), c+s*glm::vec3(-0.5f,  0.5f, -0.5f), c+s*glm::vec3(-0.5f, -0.5f, -0.5f), color),
            Triangle(c+s*glm::vec3(-0.5f, -0.5f, -0.5f), c+s*glm::vec3(-0.5f, -0.5f,  0.5f), c+s*glm::vec3(-0.5f,  0.5f,  0.5f), color),

            Triangle(c+s*glm::vec3( 0.5f,  0.5f,  0.5f), c+s*glm::vec3( 0.5f,  0.5f, -0.5f), c+s*glm::vec3( 0.5f, -0.5f, -0.5f), color),
            Triangle(c+s*glm::vec3( 0.5f, -0.5f, -0.5f), c+s*glm::vec3( 0.5f, -0.5f,  0.5f), c+s*glm::vec3( 0.5f,  0.5f,  0.5f), color),

            Triangle(c+s*glm::vec3(-0.5f, -0.5f, -0.5f), c+s*glm::vec3( 0.5f, -0.5f, -0.5f), c+s*glm::vec3( 0.5f, -0.5f,  0.5f), color),
            Triangle(c+s*glm::vec3( 0.5f, -0.5f,  0.5f), c+s*glm::vec3(-0.5f, -0.5f,  0.5f), c+s*glm::vec3(-0.5f, -0.5f, -0.5f), color),

            Triangle(c+s*glm::vec3(-0.5f,  0.5f, -0.5f), c+s*glm::vec3( 0.5f,  0.5f, -0.5f), c+s*glm::vec3( 0.5f,  0.5f,  0.5f), color),
            Triangle(c+s*glm::vec3( 0.5f,  0.5f,  0.5f), c+s*glm::vec3(-0.5f,  0.5f,  0.5f), c+s*glm::vec3(-0.5f,  0.5f, -0.5f), color)
        };
    }
};

#endif

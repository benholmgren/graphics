#ifndef _CSCI441_RENDERER_H_
#define _CSCI441_RENDERER_H_

#include "camera.h"
#include "hit.h"
#include "light.h"
#include "ray.h"
#include "intersector.h"

class Renderer {

    Intersector* _intersector;

    int clamp255(float v) {
        return std::max(0.0f, std::min(255.0f, 255.0f*v));
    }


    rgb_t to_color(const glm::vec3 c) {
        return make_colour(clamp255(c.x), clamp255(c.y), clamp255(c.z));
    }


    glm::vec3 phong(const Hit& hit, const Light* light, const glm::vec3& eye) {

        float ambientStrength = 0.2;
        float specularStrength = 0.5;
        float shinyness = 2;

        glm::vec3 pos = hit.position();
        glm::vec3 normal = hit.normal();

        glm::vec3 light_dir = glm::normalize(light->direction(pos));

        float ambient = ambientStrength;

        float diffuse = glm::max(glm::dot(normal, light_dir), 0.0f);

        glm::vec3 view_dir = glm::normalize(eye - pos);
        glm::vec3 reflectDir = glm::reflect(-light_dir, normal);
        float specular = specularStrength *
            pow(std::fmax(glm::dot(view_dir, reflectDir), 0.0), shinyness);

        glm::vec3 light_color  =
            (ambient+diffuse+specular)
            * light->attenuation(pos)
            * light->color();

        return light_color*hit.color();
    }


    glm::vec3 shade(const Camera& camera, const Lights& lights, const Hit& hit) {
        glm::vec3 color = camera.background;
        if (hit.is_intersection()) {
            color = glm::vec3(0,0,0);
            for (auto light : lights) {
                color += phong(hit, light, camera.pos);
            }
        }
        return color;
    }

    glm::vec3 render_pixel(
        const Camera& camera,
        const Lights& lights,
        const World& world,
        const Ray& ray
    ) {

        Hit hit = _intersector->find_first_intersection(world, ray);
        return shade(camera, lights, hit);
    }

public:

    Renderer(Intersector* intersector) : _intersector(intersector) { }

    void render(
        bitmap_image &image,
        const Camera& camera,
        const Lights& lights,
        const World& pposworld,
        const World& nposworld,
        const World& pnegworld,
        const World& nnegworld
    ) {
        for (int y = 0; y < image.height()/2; ++y) {
            for (int x = 0; x < image.width()/2; ++x) {
                Ray ray = camera.make_ray(image.width(), image.height(), x, y);
                glm::vec3 c = render_pixel(camera, lights, nnegworld, ray);
                image.set_pixel(x, image.height()-y-1, to_color(c));
            }
            for (int x = image.width()/2; x < image.width(); ++x) {
                Ray ray = camera.make_ray(image.width(), image.height(), x, y);
                glm::vec3 c = render_pixel(camera, lights, nposworld, ray);
                image.set_pixel(x, image.height()-y-1, to_color(c));
            }
        }
        for (int y = image.height()/2; y < image.height(); ++y) {
            for (int x = 0; x < image.width()/2; ++x) {
                Ray ray = camera.make_ray(image.width(), image.height(), x, y);
                glm::vec3 c = render_pixel(camera, lights, pnegworld, ray);
                image.set_pixel(x, image.height()-y-1, to_color(c));
            }
            for (int x = image.width()/2; x < image.width(); ++x) {
                Ray ray = camera.make_ray(image.width(), image.height(), x, y);
                glm::vec3 c = render_pixel(camera, lights, pposworld, ray);
                image.set_pixel(x, image.height()-y-1, to_color(c));
            }
        }
    }
};

#endif

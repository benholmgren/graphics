#include <vector>
#include "geometry.h"
#include "bitmap/bitmap_image.hpp"


struct Sphere {
    Vec3f center;
    float radius;
    Vec3f color;

    Sphere(const Vec3f &c, const float &r, const Vec3f &color) : center(c), radius(r), color(color) {}

    bool ray_intersect(const Vec3f &orig, const Vec3f &dir, float &t0) const {
        Vec3f L = center - orig;
        float tca = L*dir;
        float d2 = L*L - tca*tca;
        if (d2 > radius*radius) return false;
        float thc = sqrtf(radius*radius - d2);
        t0       = tca - thc;
        float t1 = tca + thc;
        if (t0 < 0) t0 = t1;
        if (t0 < 0) return false;
        return true;
    }
};

Vec3f cast_ray(const Vec3f &orig, const Vec3f &dir, const std::vector<Sphere> &spheres) {
    float spheres_dist = std::numeric_limits<float>::max();
    float dist_i;
    for (size_t i = 0; i < spheres.size(); i++) {
        if(spheres[i].ray_intersect(orig, dir, dist_i) && dist_i < spheres_dist){
            spheres_dist = dist_i;
            return spheres[i].color; //sphere color
        }
    }
    
    return Vec3f(255, 100, 200); // background color
}

void orthoRender(bitmap_image& image, const std::vector<Sphere> &spheres) {
    const int width    = 640;
    const int height   = 480;
    const int fov      = M_PI/2.;

    #pragma omp parallel for
    for (size_t j = 0; j<height; j++) {
        for (size_t i = 0; i<width; i++) {
            float x =  (2*(i + 0.5)/(float)width  - 1)*tan(fov/2.)*width/(float)height;
            float y = -(2*(j + 0.5)/(float)height - 1)*tan(fov/2.);
            Vec3f dir = Vec3f(x, y, -1).normalize();
            
            double u = 1 + ((30 - 1)*(x + 0.5))/640;
            double v = 1 + ((30 - 1)*(y + 0.5))/480;
            
            Vec3f col = cast_ray(Vec3f(u,v,0), dir, spheres);
            rgb_t color = make_colour(col[0], col[1], col[2]);
            image.set_pixel(i, j, color);
        }
    }
}

void render(bitmap_image& image, const std::vector<Sphere> &spheres) {
    const int width    = 640;
    const int height   = 480;
    const int fov      = M_PI/2.;

    #pragma omp parallel for
    for (size_t j = 0; j<height; j++) {
        for (size_t i = 0; i<width; i++) {
            float x =  (2*(i + 0.5)/(float)width  - 1)*tan(fov/2.)*width/(float)height;
            float y = -(2*(j + 0.5)/(float)height - 1)*tan(fov/2.);
            Vec3f dir = Vec3f(x, y, -1).normalize();
            Vec3f col = cast_ray(Vec3f(0,0,0), dir, spheres);
            rgb_t color = make_colour(col[0], col[1], col[2]);
            image.set_pixel(i, j, color);
        }
    }
}


int main() {
    // orthographic image
    bitmap_image orthoimage(640, 480);
    std::vector<Sphere> orthospheres;
    orthospheres.push_back(Sphere(Vec3f(-3, 0, -16), 2, Vec3f(255, 255, 0)));
    orthospheres.push_back(Sphere(Vec3f(-4, 2, -13), 2, Vec3f(255, 255, 255)));
    orthospheres.push_back(Sphere(Vec3f(5, 5, -14), 3, Vec3f(0, 215, 255)));
    orthospheres.push_back(Sphere(Vec3f(5, 2, -12), 3, Vec3f(0, 215, 55)));
    orthospheres.push_back(Sphere(Vec3f(3, -4, -17), 4, Vec3f(150, 0, 255)));
    orthoRender(orthoimage, orthospheres);
    
    orthoimage.save_image("/Users/benjaminholmgren/Desktop/Courses/lab8/orthographic.bmp");
    
    // create an image 640 pixels wide by 480 pixels tall
    bitmap_image image(640, 480);
    std::vector<Sphere> spheres;
    spheres.push_back(Sphere(Vec3f(-3, 0, -16), 2, Vec3f(255, 255, 0)));
    spheres.push_back(Sphere(Vec3f(-4, 2, -13), 2, Vec3f(255, 255, 255)));
    spheres.push_back(Sphere(Vec3f(5, 5, -14), 3, Vec3f(0, 215, 255)));
    spheres.push_back(Sphere(Vec3f(5, 2, -12), 3, Vec3f(0, 215, 55)));
    spheres.push_back(Sphere(Vec3f(3, -4, -17), 4, Vec3f(150, 0, 255)));
    render(image, spheres);
    
    image.save_image("/Users/benjaminholmgren/Desktop/Courses/lab8/perspective.bmp");
    std::cout << "Success" << std::endl;

    return 0;
}


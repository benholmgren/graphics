#include <iostream>

#include <glm/glm.hpp>

#include <bitmap/bitmap_image.hpp>

#include "camera.h"
#include "hit.h"
#include "intersector.h"
#include "light.h"
#include "ray.h"
#include "renderer.h"
#include "shape.h"
#include "timer.h"


class Bounder{
public:
    glm::vec3 mins;
    glm::vec3 maxes;
    
    Bounder(const glm::vec3& mi, const glm::vec3& ma) : mins(mi), maxes(ma) {}
};


//idea 1, create a bounder for each box, iterate through bounders first
std::vector<Bounder> bounders;

//idea 2, create bounder which contains shapes
class ShapeBounder{
public:
    std::vector<Sphere> neighborhood;
    std::vector<Triangle> box;
    
};



class BruteForceIntersector : public Intersector {
public:

    Hit find_first_intersection(const World& world, const Ray& ray) {
        Hit hit(ray);
        for (auto surface : world.shapes()) {
            double cur_t = surface->intersect(ray);

            if (cur_t < hit.t()) {
                hit.update(surface, cur_t);
            }
        }
        return hit;
    }
};

/*
class MySlickIntersector : public Intersector {
public:
    std::vector<ShapeBounder> shapebounders;
    
    Hit find_first_intersection(const World& world, const Ray& ray) {
        Hit hit(ray);
        double best_boun_t = hit.t();
        ShapeBounder theone;
        for (auto bounder : shapebounders) {
            for (auto triangle : bounder.box) {
                double boun_t = triangle.Shape::intersect(ray);
                if (boun_t < best_boun_t) {
                    best_boun_t = boun_t;
                    theone.neighborhood = bounder.neighborhood;
                    theone.box = bounder.box;
                }
            }
        }
        World sphworld;
        for (auto sphere : theone.neighborhood) {
            sphworld.append(sphere);
        }
        sphworld.lock();
        for (auto surface : sphworld.shapes()) {
            double cur_t = surface->intersect(ray);
                if (cur_t < hit.t()) {
                    hit.update(surface, cur_t);
                }
            }
        
        return hit;
    }
};
*/

double rand_val() {
    static bool init = true;
    if (init) {
        srand(time(NULL));
        init = false;
    }
    return ((double) rand() / (RAND_MAX));
}

glm::vec3 rand_color() {
    return glm::vec3(rand_val(),rand_val(),rand_val());
}


std::vector<Triangle> random_box() {
    float  x = (rand_val() * 8) - 4;
    float  y = (rand_val() * 8) - 4;
    float  z = rand_val() * 5;
    float scale = rand_val() * 2;
    std::vector<Triangle> box = Obj::make_box(glm::vec3(x, y, z), scale, rand_color());
    // we know that the box is made the same way every time, so find the maximal
    // x,y,z with indexing
    float minx = box[0]._a[0];
    float miny = box[0]._a[1];
    float minz = box[0]._a[2];
    float maxx = box[6]._a[0];
    float maxy = box[6]._a[1];
    float maxz = box[6]._a[2];
    
    bounders.push_back(Bounder(glm::vec3(minx,miny, minz), glm::vec3(maxx, maxy, maxz)));
    return box;
}


int main(int argc, char** argv) {

    // set the number of boxes
    int NUM_BOXES = 1;

    // create an image 640 pixels wide by 480 pixels tall
    bitmap_image image(640, 480);

    // setup the camera
    float dist_to_origin = 5;
    Camera camera(
            glm::vec3(0, 0, -dist_to_origin),   // eye
            glm::vec3(0, 0, 0),                 // target
            glm::vec3(0, 1, 0),                 // up
            glm::vec2(-5, -5),                  // viewport min
            glm::vec2(5, 5),                    // viewport max
            dist_to_origin,                     // distance from eye to view plane
            glm::vec3(.3, .6, .8)               // background color
    );

    // setup lights
    // see http://wiki.ogre3d.org/tiki-index.php?page=-Point+Light+Attenuation
    // for good attenuation value.
    // I found the values at 7 to be nice looking
    PointLight l1(glm::vec3(1, 1, 1), glm::vec3(3, -3, 0), 1.0, .7, 0.18);
    DirectionalLight l2(glm::vec3(.5, .5, .5), glm::vec3(-5, 4, -1));
    Lights lights = { &l1, &l2 };

    // setup world
    World pposworld;
    World nposworld;
    World pnegworld;
    World nnegworld;

    // add the light
    pposworld.append(Sphere(l1.position(), .25, glm::vec3(1,1,1)));

    // and the spheres
    Sphere s1 = Sphere(glm::vec3(1, 1, 1), 1, rand_color());
    pposworld.append(s1);
    Sphere s2 = Sphere(glm::vec3(2, 2, 4), 2, rand_color());
    pposworld.append(s2);
    Sphere s3 = Sphere(glm::vec3(3, 3, 6), 3, rand_color());
    pposworld.append(s3);
    
    
    //generate box for bounding spheres
    std::vector<Triangle> spherebox = Obj::make_box(glm::vec3(3.0f, 3.0f, 4.5f), 6, rand_color());
    //make intersector so we can incude bounding boxes
    //MySlickIntersector sphereintersector;
    
    ShapeBounder spherebound;
    spherebound.box = spherebox;
    std::vector<Sphere> hood {s1, s2, s3};
    spherebound.neighborhood = hood;
    
    //sphereintersector.shapebounders.push_back(spherebound);
    
    
    
    // and add some boxes and prep world for rendering
    for (int i = 0 ; i < NUM_BOXES ; ++i) {
        //world.append(random_box());
        float  x = (rand_val() * 8) - 4;
        float  y = (rand_val() * 8) - 4;
        float  z = rand_val() * 5;
        float scale = rand_val() * 2;
        if (x >= 0) {
            if (y >= 0) {
                pposworld.append(Obj::make_box(glm::vec3(x, y, z), scale, rand_color()));
            }
            else{
                pposworld.append(Obj::make_box(glm::vec3(x, y, z), scale, rand_color()));
            }

        }
        else if(x< 0){
            if (y >= 0) {
                pnegworld.append(Obj::make_box(glm::vec3(x, y, z), scale, rand_color()));
            }
            else{
                nnegworld.append(Obj::make_box(glm::vec3(x, y, z), scale, rand_color()));
            }
        }
    }
    pposworld.lock();
    nposworld.lock();
    pnegworld.lock();
    nnegworld.lock();
    // create the intersector
    BruteForceIntersector bruteintersector;
    

    // and setup the renderer
    Renderer renderer(&bruteintersector);
    
    //slick intersection
    //Renderer renderer(&sphereintersector);

    // render
    Timer timer;
    timer.start();
    renderer.render(image, camera, lights, pposworld, nposworld, pnegworld, nnegworld);
    timer.stop();

    image.save_image("/Users/benjaminholmgren/Desktop/Courses/graphics/lab9/lab9/img/ray-traced.bmp");
    std::cout << "Rendered in " <<  timer.total() << " milliseconds" << std::endl;
}



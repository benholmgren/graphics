#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <cmath>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <csci441/shader.h>
#include <csci441/matrix4.h>
#include <csci441/matrix3.h>
#include <csci441/vector4.h>
#include <csci441/uniform.h>

#include "shape.h"
#include "model.h"
#include "camera.h"
#include "renderer.h"

double move = 0.01;
double xpos = -0.9;
double ypos = -0.9;
double zpos = 0.94;

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 960;

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

bool isPressed(GLFWwindow *window, int key) {
    return glfwGetKey(window, key) == GLFW_PRESS;
}

bool isReleased(GLFWwindow *window, int key) {
    return glfwGetKey(window, key) == GLFW_RELEASE;
}

Matrix4 processModel(const Matrix4& model, GLFWwindow *window) {
    Matrix4 trans;

    const float ROT = 1;
    const float SCALE = .05;
    const float TRANS = .01;

    // ROTATE
    if (isPressed(window, GLFW_KEY_U)) { trans.rotate_x(-ROT); }
    else if (isPressed(window, GLFW_KEY_I)) { trans.rotate_x(ROT); }
    else if (isPressed(window, GLFW_KEY_O)) { trans.rotate_y(-ROT); }
    else if (isPressed(window, GLFW_KEY_P)) { trans.rotate_y(ROT); }
    else if (isPressed(window, '[')) { trans.rotate_z(-ROT); }
    else if (isPressed(window, ']')) { trans.rotate_z(ROT); }
    // SCALE
    else if (isPressed(window, '-')) { trans.scale(1-SCALE, 1-SCALE, 1-SCALE); }
    else if (isPressed(window, '=')) { trans.scale(1+SCALE, 1+SCALE, 1+SCALE); }
    // TRANSLATE
    else if (isPressed(window, GLFW_KEY_UP)) { trans.translate(0, TRANS, 0); }
    else if (isPressed(window, GLFW_KEY_DOWN)) { trans.translate(0, -TRANS, 0); }
    else if (isPressed(window, GLFW_KEY_LEFT)) { trans.translate(-TRANS, 0, 0); }
    else if (isPressed(window, GLFW_KEY_RIGHT)) { trans.translate(TRANS, 0, 0); }
    else if (isPressed(window, ',')) { trans.translate(0,0,TRANS); }
    else if (isPressed(window, '.')) { trans.translate(0,0,-TRANS); }

    return trans * model;
}

void processInput(Matrix4& model, GLFWwindow *window) {
    if (isPressed(window, GLFW_KEY_ESCAPE) || isPressed(window, GLFW_KEY_Q)) {
        glfwSetWindowShouldClose(window, true);
    }
    model = processModel(model, window);
}

void errorCallback(int error, const char* description) {
    fprintf(stderr, "GLFW Error: %s\n", description);
}


int main(void) {
    GLFWwindow* window;

    glfwSetErrorCallback(errorCallback);

    /* Initialize the library */
    if (!glfwInit()) { return -1; }

    glfwWindowHint (GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "CSCI441-lab", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    
    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    // tell glfw what to do on resize
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    // init glad
    if (!gladLoadGL()) {
        std::cerr << "Failed to initialize OpenGL context" << std::endl;
        glfwTerminate();
        return -1;
    }

    // create maze
    Model maze(
            MazeFloor().coords,
            Shader("../vert.glsl", "../frag.glsl"));
    Matrix4 maze_roty;
    maze_roty.rotate_y(180);
    maze.model = maze_roty;
    

    // make a floor
    float init[3] = {0.90, 0, -0.94};
    Model protagonist(
            DiscoCube().coords,
            Shader("../vert.glsl", "../frag.glsl"));
    Matrix4 pro_trans, pro_roty;
    pro_trans.translate(init[0], init[1], init[2]);
    pro_roty.rotate_y(180);
    protagonist.model = pro_roty*pro_trans;

    // setup camera
    Matrix4 projection;
    projection.perspective(45, 1, .01, 10);

    Camera regular;
    regular.projection = projection;
    regular.eye = Vector4(0, 0, 3);
    regular.origin = Vector4(0, 0, 0);
    regular.up = Vector4(0, 1, 0);

    // and use z-buffering
    glEnable(GL_DEPTH_TEST);

    // create a renderer
    Renderer renderer;

    // camera in box
    Camera inside;
    inside.projection = projection;
    inside.eye = Vector4(-0.9,-0.9,0.94);
    inside.origin = Vector4(-0.9,-0.9,-0.94);
    inside.up = Vector4(0,1,0);

    // set the light position
    Vector4 lightPos(3.75f, 3.75f, 4.0f);

    //Camera matrix
    Camera camera;
    camera = regular;
    float angle = 0;
    int count = 0;
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {
        // process input
        processInput(maze.model, window);
        processInput(protagonist.model, window);

        /* Render here */
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        if(count%2 == 0){
            camera = regular;
        }
        else{
        camera = inside;
        }
        
        Matrix4 pro_transl;
        if (isPressed(window, GLFW_KEY_W)) {
            //implement collision checking
            //move everything back
            if (-0.95 >= zpos) {
                pro_transl.translate(0,0,-move);
                zpos += move;
                inside.eye = inside.eye + Vector4(0,0,move);
                inside.origin = inside.origin + Vector4(0,0,move);
            }
            //if everything is not collided
            else{
                pro_transl.translate(0,0,move);
                zpos -= move;
                inside.eye = inside.eye + Vector4(0,0,-move);
                inside.origin = inside.origin + Vector4(0,0,-move);
            }
        }
        else if (isPressed(window, GLFW_KEY_S)) {
            //more collision checking
            if (0.95 <= zpos) {
                pro_transl.translate(0,0, move);
                zpos -= move;
                inside.eye = inside.eye + Vector4(0,0,-move);
                inside.origin = inside.origin + Vector4(0,0,-move);
            }
            else{
                pro_transl.translate(0,0, -move);
                zpos += move;
                inside.eye = inside.eye + Vector4(0,0,move);
                inside.origin = inside.origin + Vector4(0,0,move);
            }
        }
        else if (isPressed(window, GLFW_KEY_A)) {
            pro_transl.translate(move,0,0);
            inside.eye = inside.eye + Vector4(-move,0,0);
            inside.origin = inside.origin + Vector4(-move,0,0);

        }
        else if (isPressed(window, GLFW_KEY_D)) {
            pro_transl.translate(-move, 0, 0);
            inside.eye = inside.eye + Vector4(move,0,0);
            inside.origin = inside.origin + Vector4(move,0,0);

        }
        
        if (isPressed(window, GLFW_KEY_SPACE)){
		count++;
        }
        else if (isPressed(window, GLFW_KEY_F)){
		angle = angle + 5*move;
		inside.origin = Vector4(100*cos(angle),-0.9,100*sin(angle));
        }
        else if (isPressed(window, GLFW_KEY_G)){
		angle = angle - 5*move;
		inside.origin = Vector4(100*cos(angle),-0.9,100*sin(angle));
        }
        
        

		
        
        protagonist.model = protagonist.model * pro_transl;
        // render the object and the floor
        renderer.render(camera, maze, lightPos);
        renderer.render(camera, protagonist, lightPos);

        /* Swap front and back and poll for io events */
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}


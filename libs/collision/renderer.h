/*
 * Copyright (c) Tim Broenink and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

//
// Created by Tim on 19/06/2017.
//

#ifndef COLLISION_SIM_GLDEBUGDRAW_H
#define COLLISION_SIM_GLDEBUGDRAW_H

#include <cstdio>
#include <iostream>

#include <LinearMath/btIDebugDraw.h>

#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <vector>

#include "collision_body.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "shader.h"
#include "Camera.h"

#include "renderable.h"
#include "loadMeshFromSTL.h"


class renderer: public btIDebugDraw {

public:
    ~renderer() override;
    void	drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) override;
    void	reportErrorWarning(const char* warningString) override;
    void	draw3dText(const btVector3& location, const char* textString) override;
    void	setDebugMode(int debugMode) override;
    int		getDebugMode() const override;
    void 	drawLine (const btVector3 &from, const btVector3 &to, const btVector3 &color) override;

    void mouse_callback(GLFWwindow *window, double xpos, double ypos);

    int init();
    bool render();

    bool addRenderable(Renderable*);

private:
    GLFWwindow* window;
    unsigned int VBO,VAO,length;
    std::vector<float> points;

    std::vector<Renderable*> renders;

    float cube[216] = {
            -100.0f, -100.0f, -100.0f, 0.0f, 0.0f, -1.0f, 
            -100.0f,  100.0f, -100.0f, 0.0f, 0.0f, -1.0f, 
             100.0f,  100.0f, -100.0f, 0.0f, 1.0f, 1.0f, 
            -100.0f, -100.0f, -100.0f, 0.0f, 0.0f, -1.0f, 
            100.0f,  -100.0f, -100.0f, 0.0f, 0.0f, -1.0f, 
            100.0f,   100.0f, -100.0f, 0.0f, 1.0f, 1.0f, 

            -100.0f, -100.0f,  100.0f, 0.0f, 0.0f, 1.0f, 
            -100.0f,  100.0f,  100.0f, 0.0f, 0.0f, 1.0f, 
             100.0f,  100.0f,  100.0f, 0.0f, 0.0f, 1.0f, 
            -100.0f, -100.0f,  100.0f, 0.0f, 0.0f, 1.0f, 
            100.0f,  -100.0f,  100.0f, 0.0f, 0.0f, 1.0f, 
            100.0f,   100.0f,  100.0f, 0.0f, 0.0f, 1.0f, 

            -100.0f, -100.0f, -100.0f, 0.0f, -1.0f, 0.0f, 
            -100.0f, -100.0f,  100.0f, 0.0f, -1.0f, 0.0f, 
             100.0f, -100.0f,  100.0f, 0.0f, -1.0f, 0.0f, 
            -100.0f, -100.0f, -100.0f, 0.0f, -1.0f, 0.0f, 
             100.0f, -100.0f, -100.0f, 0.0f, -1.0f, 0.0f, 
             100.0f, -100.0f,  100.0f, 0.0f, -1.0f, 0.0f, 

            -100.0f,  100.0f, -100.0f, 0.0f, 1.0f, 0.0f, 
            -100.0f,  100.0f,  100.0f, 0.0f, 1.0f, 0.0f, 
             100.0f,  100.0f,  100.0f, 0.0f, 1.0f, 0.0f, 
            -100.0f,  100.0f, -100.0f, 0.0f, 1.0f, 0.0f, 
             100.0f,  100.0f, -100.0f, 0.0f, 1.0f, 0.0f, 
             100.0f,  100.0f,  100.0f, 0.0f, 1.0f, 0.0f, 

            -100.0f, -100.0f, -100.0f, -1.0f, 0.0f, 0.0f, 
            -100.0f, -100.0f,  100.0f, -1.0f, 0.0f, 0.0f, 
            -100.0f,  100.0f,  100.0f, -1.0f, 0.0f, 0.0f, 
            -100.0f, -100.0f, -100.0f, -1.0f, 0.0f, 0.0f, 
            -100.0f,  100.0f, -100.0f, -1.0f, 0.0f, 0.0f, 
            -100.0f,  100.0f,  100.0f, -1.0f, 0.0f, 0.0f, 

             100.0f, -100.0f, -100.0f, 1.0f, 0.0f, 0.0f, 
             100.0f, -100.0f,  100.0f, 1.0f, 0.0f, 0.0f, 
             100.0f,  100.0f,  100.0f, 1.0f, 0.0f, 0.0f, 
             100.0f, -100.0f, -100.0f, 1.0f, 0.0f, 0.0f, 
             100.0f,  100.0f, -100.0f, 1.0f, 0.0f, 0.0f, 
             100.0f,  100.0f,  100.0f, 1.0f, 0.0f, 0.0f, 
    };

    glm::mat4 perspective;
    void processInput(GLFWwindow *window);

    Shader shader,lineshader;
    Camera camera;

    float deltaTime = 0.0f;	// time between current frame and last frame
    float lastFrame = 0.0f;

    float lastX = 800 / 2.0f;
    float lastY = 600 / 2.0f;
    bool firstMouse = true;

};

#endif //COLLISION_SIM_GLDEBUGDRAW_H

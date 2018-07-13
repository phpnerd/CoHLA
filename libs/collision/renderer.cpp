/*
 * Copyright (c) Tim Broenink and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

//
// Created by Tim on 19/06/2017.
//

#include "renderer.h"

renderer::~renderer() {
    glfwDestroyWindow(window);
    delete window;
}

void renderer::drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color) {
    points.push_back(from.x());
    points.push_back(from.y());
    points.push_back(from.z());
    points.push_back(color.x());
    points.push_back(color.y());
    points.push_back(color.z());
    points.push_back(to.x());
    points.push_back(to.y());
    points.push_back(to.z());
    points.push_back(color.x());
    points.push_back(color.y());
    points.push_back(color.z());
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

void renderer::processInput(GLFWwindow *window) {

    Camera_Speed camera_speed;

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)== GLFW_PRESS)
        camera_speed = FAST;
    else
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL)== GLFW_PRESS)
        camera_speed = SLOW;
    else
    if (glfwGetKey(window, GLFW_KEY_LEFT_ALT)== GLFW_PRESS)
        camera_speed = VERY_SLOW;
    else
        camera_speed = DEFAULT;


    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime, camera_speed);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime, camera_speed);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime, camera_speed);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime, camera_speed);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime, camera_speed);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime, camera_speed);
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        camera.ProcessKeyboard(PITCHUP, deltaTime, camera_speed);
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        camera.ProcessKeyboard(PITCHDOWN, deltaTime, camera_speed);
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        camera.ProcessKeyboard(YAWLEFT, deltaTime, camera_speed);
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        camera.ProcessKeyboard(YAWRIGHT, deltaTime, camera_speed);
}
// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------

void renderer::mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = (float) xpos;
        lastY = (float) ypos;
        firstMouse = false;
    }

    float xoffset = (float) xpos - lastX;
    float yoffset = lastY - (float) ypos; // reversed since y-coordinates go from bottom to top

    lastX = (float) xpos;
    lastY = (float) ypos;

    camera.ProcessMouseMovement(xoffset * 4, yoffset * 4);
}

int renderer::init() {
    printf("Creating Window\n glfw version: %s\n", glfwGetVersionString());

    if (!glfwInit()) {
        wcerr << "Failed to initialise GLFW" << endl;
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X




    // glfw window creation
    // --------------------
    window = glfwCreateWindow(800, 600, "CollisionSimulator", nullptr, nullptr);
    if (window == nullptr) {
        wcerr << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    //glfwSetCursorPosCallback(window, (static_mouse_callback));
    //glfwSetScrollCallback(window, (this->scroll_callback));

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);


    // build and compile our shader program
    // ------------------------------------
    // vertex shader
    shader = Shader(SHADER_VS_LIGHTING, SHADER_FS_LIGHTING);
    lineshader = Shader(SHADER_VS_SIMPLE, SHADER_FS_SIMPLE);
    camera = Camera(glm::vec3(0.0f, 100.0f, 1000.0f));


    //Setup Viewing matices
    perspective = glm::perspective(glm::radians(45.0f), 800.0f / 600, 10.0f, 100000.0f);


    // Create Test Cube
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------

    return 0;
}

bool renderer::render() {
    float currentFrame = (float) glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    //input
    processInput(window);

    //render
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    shader.use();
    shader.setMat4("view", camera.GetViewMatrix());
    shader.setMat4("projection", perspective);
    shader.setVec3("lightColor", glm::vec3(1.0, 1.0, 1.0));
    shader.setVec3("lightPos", glm::vec3(1000, 1000, 1000));
    shader.setVec3("viewPos", camera.Position);
    glEnable(GL_BLEND);
    for (auto r:renders) {
        glm::mat4 transform = r->getTransform();
        shader.setMat4("model", transform);
        shader.setMat3("modelnorm", glm::transpose(glm::inverse(transform)));
        shader.setVec3("objectColor", r->getColor().toGLMVector3());

        glBindVertexArray(r->getMesh());
        glDrawArrays(GL_TRIANGLES, 0, r->getLength() * sizeof(float));
    }

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(float), points.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.

    lineshader.use();
    lineshader.setMat4("view", camera.GetViewMatrix());
    lineshader.setMat4("projection", perspective);
    glDrawArrays(GL_LINES, 0, points.size() * sizeof(float));
    glBindVertexArray(0);
    points.clear();

    //finish rendering
    glfwSwapBuffers(window);
    glfwPollEvents();

    points.clear();
    return !glfwWindowShouldClose(window);
}

void renderer::setDebugMode(int debugMode) {

}

void renderer::draw3dText(const btVector3 &location, const char *textString) {

}

void renderer::reportErrorWarning(const char *warningString) {
    std::cout << warningString << std::endl;
}

void renderer::drawContactPoint(const btVector3 &PointOnB, const btVector3 &normalOnB, btScalar distance, int lifeTime,
                                const btVector3 &color) {
    wcout << "ContactPoint: (" << PointOnB.getX() << ", " << PointOnB.getY() << ", " << PointOnB.getZ() << ")" << endl;
    wcout.flush();
}

int renderer::getDebugMode() const {
    return DBG_NoDebug;//DBG_DrawAabb | DBG_DrawWireframe | DBG_DrawContactPoints;
}

bool renderer::addRenderable(Renderable *r) {
    renders.push_back(r);
    return true;
}
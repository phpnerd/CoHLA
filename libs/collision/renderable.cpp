/*
 * Copyright (c) Tim Broenink and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

//
// Created by Tim on 30/06/2017.
//

#include "renderable.h"

Renderable::Renderable(Vector &color, glm::mat4 transform, unsigned int vba, size_t length) :
        color(color),
        transform(transform),
        vba(vba),
        length(length)
{}

Vector Renderable::getColor() {
    return color;
}

unsigned int Renderable::getMesh() {
    return vba;
}

glm::mat4 Renderable::getTransform() {
    return transform;
}

unsigned int Renderable::meshCube(float sx, float sy, float sz) {
    unsigned int VAO, VBO;
    float cube[216] = {
            -sx, -sy, -sz,  0.0f,  0.0f, -1.0f,
            -sx,  sy, -sz,  0.0f,  0.0f, -1.0f,
             sx,  sy, -sz,  0.0f,  1.0f,  1.0f,
            -sx, -sy, -sz,  0.0f,  0.0f, -1.0f,
             sx, -sy, -sz,  0.0f,  0.0f, -1.0f,
             sx,  sy, -sz,  0.0f,  1.0f,  1.0f,

            -sx, -sy,  sz,  0.0f,  0.0f,  1.0f,
            -sx,  sy,  sz,  0.0f,  0.0f,  1.0f,
             sx,  sy,  sz,  0.0f,  0.0f,  1.0f,
            -sx, -sy,  sz,  0.0f,  0.0f,  1.0f,
             sx, -sy,  sz,  0.0f,  0.0f,  1.0f,
             sx,  sy,  sz,  0.0f,  0.0f,  1.0f,

            -sx, -sy, -sz,  0.0f, -1.0f,  0.0f,
            -sx, -sy,  sz,  0.0f, -1.0f,  0.0f,
             sx, -sy,  sz,  0.0f, -1.0f,  0.0f,
            -sx, -sy, -sz,  0.0f, -1.0f,  0.0f,
             sx, -sy, -sz,  0.0f, -1.0f,  0.0f,
             sx, -sy,  sz,  0.0f, -1.0f,  0.0f,

            -sx,  sy, -sz,  0.0f,  1.0f,  0.0f,
            -sx,  sy,  sz,  0.0f,  1.0f,  0.0f,
             sx,  sy,  sz,  0.0f,  1.0f,  0.0f,
            -sx,  sy, -sz,  0.0f,  1.0f,  0.0f,
             sx,  sy, -sz,  0.0f,  1.0f,  0.0f,
             sx,  sy,  sz,  0.0f,  1.0f,  0.0f,

            -sx, -sy, -sz, -1.0f,  0.0f,  0.0f,
            -sx, -sy,  sz, -1.0f,  0.0f,  0.0f,
            -sx,  sy,  sz, -1.0f,  0.0f,  0.0f,
            -sx, -sy, -sz, -1.0f,  0.0f,  0.0f,
            -sx,  sy, -sz, -1.0f,  0.0f,  0.0f,
            -sx,  sy,  sz, -1.0f,  0.0f,  0.0f,

             sx, -sy, -sz,  1.0f,  0.0f,  0.0f,
             sx, -sy,  sz,  1.0f,  0.0f,  0.0f,
             sx,  sy,  sz,  1.0f,  0.0f,  0.0f,
             sx, -sy, -sz,  1.0f,  0.0f,  0.0f,
             sx,  sy, -sz,  1.0f,  0.0f,  0.0f,
             sx,  sy,  sz,  1.0f,  0.0f,  0.0f
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 216 * sizeof(float), cube, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);
    return VAO;
}

unsigned int Renderable::meshVector(std::vector<float> v) {
    unsigned int VAO, VBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, v.size() * sizeof(float), v.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);
    return VAO;
}

size_t Renderable::getLength() {
    return length;
}

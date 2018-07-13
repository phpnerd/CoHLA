/*
 * Copyright (c) Tim Broenink and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

//
// Created by Tim on 30/06/2017.
//

#ifndef OPENRTI_LIB_RENDERABLE_H
#define OPENRTI_LIB_RENDERABLE_H

#include "shader.h"
#include "Vector.h"


class Renderable {

public:
    Renderable(Vector&, glm::mat4, unsigned int, size_t);
    Vector getColor();
    unsigned int getMesh();
    virtual glm::mat4 getTransform();
    size_t getLength();

    static unsigned int meshCube(float, float, float);
    static unsigned int meshVector(std::vector<float>);

private:
    Vector color;
    unsigned int vba;
    size_t length;
    glm::mat4 transform;
};


#endif //OPENRTI_LIB_RENDERABLE_H

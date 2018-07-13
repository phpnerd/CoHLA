/*
 * Copyright (c) Tim Broenink and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

//
// Created by thomas on 8-1-18.
//

#ifndef OPENRTI_LIB_VECTOR_H
#define OPENRTI_LIB_VECTOR_H

#include <iostream>
#include <vector>
#include <LinearMath/btVector3.h>
#include <glm/vec3.hpp>

#include "boost/property_tree/ptree.hpp"

using namespace std;
using boost::property_tree::ptree;

class Vector {

public:
    Vector();
    Vector(vector<float>&);
    Vector(float, float, float);
    float getX(), getY(), getZ(), _get(int);
    void setX(float), setY(float), setZ(float), _set(int, float);
    unsigned long size();

    btVector3 toVector3();
    glm::vec3 toGLMVector3();

    string toString();

    static Vector parse(ptree&);

private:
    vector<float> values;

};


#endif //OPENRTI_LIB_VECTOR_H

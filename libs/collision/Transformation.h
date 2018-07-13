/*
 * Copyright (c) Tim Broenink and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

//
// Created by thomas on 8-1-18.
//

#ifndef OPENRTI_LIB_TRANSFORMATION_H
#define OPENRTI_LIB_TRANSFORMATION_H

#define KEYWORD_ORIGIN      "origin"
#define KEYWORD_ROTATION    "rotation"
#define KEYWORD_STATE       "state"

#include <string>

#include "boost/property_tree/ptree.hpp"
#include "btBulletCollisionCommon.h"

#include "transform.h"
#include "Vector.h"

using namespace std;
using boost::property_tree::ptree;

class Transformation {

public:
    Transformation(Vector&, Vector&, string&);

    Transform* getTransform();

    void buildTransform();

    string toString(string prefix = "");

    static Transformation parse(ptree&);

private:
    Vector origin, rotation;
    string state;

    Transform* transform = nullptr;

};


#endif //OPENRTI_LIB_TRANSFORMATION_H

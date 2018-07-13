/*
 * Copyright (c) Tim Broenink and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

//
// Created by Tim on 13/06/17.
//

#ifndef COLLISION_SIM_TRANSFORM_H
#define COLLISION_SIM_TRANSFORM_H

#include <LinearMath/btVector3.h>
#include <LinearMath/btTransform.h>
#include <string>
#include <map>

using namespace std;

class Transform {

public:
    Transform();
    void addTransformState(string, btTransform*);
    void calculate(map<string, float>, btTransform*);

private:
    std::map<string, btTransform*> transforms;

};

#endif //COLLISION_SIM_TRANSFORM_H
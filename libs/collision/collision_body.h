/*
 * Copyright (c) Tim Broenink and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

//
// Created by Tim on 13/06/17.
//

#ifndef COLLISION_SIM_COLLISIONBODY_H
#define COLLISION_SIM_COLLISIONBODY_H

#include <BulletCollision/CollisionDispatch/btCollisionObject.h>
#include <string>
#include <map>
#include <list>

#include "glm/glm.hpp"

#include "transform.h"

using namespace std;

class CollisionBody {

public:
    CollisionBody(btCollisionObject*);
    void updateTransform(map<string, float>);
    void addTransform(Transform*);
    btCollisionObject* collisionObject;
private:
    list<Transform*> transforms;
};

#endif //COLLISION_SIM_COLLISIONBODY_H

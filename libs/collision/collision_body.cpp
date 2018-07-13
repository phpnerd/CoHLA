/*
 * Copyright (c) Tim Broenink and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

//
// Created by Tim on 13/06/17.
//

#include "collision_body.h"

CollisionBody::CollisionBody(btCollisionObject* co) {
    collisionObject = co;
}

void CollisionBody::updateTransform(map<string, float> statemap) {
    btTransform tf = collisionObject->getWorldTransform();
    btTransform* temp = new btTransform;
    tf.setIdentity();
    for (auto t : transforms) {
        t->calculate(statemap, temp);
        tf*=*temp;
    }
    collisionObject->setWorldTransform(tf);
}

void CollisionBody::addTransform(Transform* t) {
    transforms.push_back(t);
}
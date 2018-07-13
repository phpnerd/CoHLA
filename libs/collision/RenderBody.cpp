/*
 * Copyright (c) Tim Broenink and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

//
// Created by Tim on 30/06/2017.
//

#include "RenderBody.h"

RenderBody::RenderBody(Vector &color, CollisionBody *b, unsigned int vba, size_t length, Vector &offset,
                       Vector &rotation) :
        Renderable(color, glm::mat4(), vba, length),
        body(b),
        offset(offset),
        rotation(rotation) {}

glm::mat4 RenderBody::getTransform() {
    float tmp[16];
    btTransform o = btTransform();
    o.setIdentity();
    o.setOrigin(offset.toVector3());
    btQuaternion rot;
    rot.setEuler(rotation.getX(), rotation.getY(), rotation.getZ());
    o.setRotation(rot);
    (body->collisionObject->getWorldTransform() * o).getOpenGLMatrix(tmp);
    return glm::make_mat4(tmp);
}

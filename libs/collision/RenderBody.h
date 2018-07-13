/*
 * Copyright (c) Tim Broenink and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

//
// Created by Tim on 30/06/2017.
//

#ifndef OPENRTI_LIB_RENDERBODY_H
#define OPENRTI_LIB_RENDERBODY_H

#include "renderable.h"
#include "collision_body.h"

class RenderBody: public Renderable {

public:
    RenderBody(Vector&, CollisionBody*, unsigned int, size_t, Vector& offset, Vector& rotation);
    glm::mat4 getTransform() override;

private:
    CollisionBody* body;
    Vector offset, rotation;

};

#endif //OPENRTI_LIB_RENDERBODY_H
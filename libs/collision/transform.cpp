/*
 * Copyright (c) Tim Broenink and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

//
// Created by Tim on 13/06/17.
//

#include "transform.h"

Transform::Transform() {}

void Transform::addTransformState(string name, btTransform* tf) {
    transforms[name] = tf;
}

void Transform::calculate(map<string, float> statemap, btTransform* t) {
    t->setIdentity();
    if (transforms[""]) {
        t->setOrigin(transforms[""]->getOrigin());
        t->setRotation(transforms[""]->getRotation());
    } else
        t->setIdentity();
    for (auto state:statemap){
        if (transforms[state.first]) {
            t->setOrigin(t->getOrigin() + transforms[state.first]->getOrigin() * state.second);
            btQuaternion tempRotation = transforms[state.first]->getRotation().normalize();
            tempRotation.setRotation(tempRotation.getAxis(),tempRotation.getAngle()*state.second);
            tempRotation.normalize();
            t->setRotation(t->getRotation()*tempRotation);
        }
    }
}
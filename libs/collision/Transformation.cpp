/*
 * Copyright (c) Tim Broenink and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

//
// Created by thomas on 8-1-18.
//

#include "Transformation.h"

Transformation::Transformation(Vector &origin, Vector &rotation, string &state) :
        origin(origin),
        rotation(rotation),
        state(state) {}

Transform *Transformation::getTransform() {
    if (!transform)
        buildTransform();
    return transform;
}

void Transformation::buildTransform() {
    btQuaternion btRotation;
    btRotation.setEuler(rotation.getX(), rotation.getY(), rotation.getZ());
    auto _btTransform = new btTransform();
    _btTransform->setIdentity();
    _btTransform->setOrigin(origin.toVector3());
    _btTransform->setRotation(btRotation);
    auto transform = new Transform;
    transform->addTransformState(state, _btTransform);
    this->transform = transform;
}

string Transformation::toString(string prefix) {
    string s = prefix + "Transformation {\n";
    s += prefix + "\torigin: " + origin.toString() + "\n";
    s += prefix + "\trotation: " + rotation.toString() + "\n";
    s += prefix + "\tstate: " + state + "\n";
    s += prefix + "}";
    return s;
}

Transformation Transformation::parse(ptree &tree) {
    Vector origin, rotation;
    string state;

    auto origin_node = tree.get_child_optional(KEYWORD_ORIGIN);
    auto rotation_node = tree.get_child_optional(KEYWORD_ROTATION);
    auto state_node = tree.get_child_optional(KEYWORD_STATE);

    if (origin_node.is_initialized())
        origin = Vector::parse(origin_node.get());
    if (rotation_node.is_initialized())
        rotation = Vector::parse(rotation_node.get());
    if (rotation.size() != 3)
        wcerr << "Invalid rotation value: requires 3 floats!" << endl;
    if (state_node.is_initialized())
        state = state_node.get().get_value<string>();

    return { origin, rotation, state };
}
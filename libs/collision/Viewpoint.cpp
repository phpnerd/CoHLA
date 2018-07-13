/*
 * Copyright (c) Tim Broenink and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

//
// Created by thomas on 8-1-18.
//

#include "Viewpoint.h"

Viewpoint::Viewpoint():
    position(0, 100, 1000),
    lookAt(0, 0, 0)
{}

Viewpoint::Viewpoint(float x1, float y1, float z1, float x2, float y2, float z2):
    position(x1, y1, z1),
    lookAt(x2, y2, z2)
{}

Viewpoint::Viewpoint(Vector& position, Vector& lookAt):
    position(position),
    lookAt(lookAt)
{}

Vector Viewpoint::getPosition() {
    return position;
}

void Viewpoint::setPosition(Vector& position) {
    this->position = position;
}

Vector Viewpoint::getLookAt() {
    return lookAt;
}

void Viewpoint::setLookAt(Vector& lookAt) {
    this->lookAt = lookAt;
}

Viewpoint Viewpoint::parse(ptree& tree) {
    auto position_node = tree.get_child_optional(KEYWORD_POSITION);
    auto lookAt_node = tree.get_child_optional(KEYWORD_LOOKAT);
    if (!(position_node.is_initialized() && lookAt_node.is_initialized()))
        wcerr << "Failed to parse Viewpoint: not all arguments were found!" << endl;
    auto position = Vector::parse(position_node.get());
    auto lookAt = Vector::parse(lookAt_node.get());
    return { position, lookAt };
}

string Viewpoint::toString(string prefix) {
    return "{\n" + prefix + "\tposition: " + position.toString() + "\n" + prefix + "\tlookAt: " + lookAt.toString() + "\n" + prefix + "}";
}
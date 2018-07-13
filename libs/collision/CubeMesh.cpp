/*
 * Copyright (c) Tim Broenink and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

//
// Created by thomas on 9-1-18.
//

#include "CubeMesh.h"

CubeMesh::CubeMesh(string &name, Vector &offset, Vector &rotation, bool is_collider, bool do_render, ptree &tree) :
        Mesh(name, offset, rotation, is_collider, do_render) {
    parse(tree);
}

btCollisionShape *CubeMesh::getCollisionShape() {
    return new btBoxShape(size.toVector3());
}

Renderable *CubeMesh::getRenderable(Vector &color, CollisionBody *collisionBody) {
    return new RenderBody(color, collisionBody,
                          Renderable::meshCube(size.getX(), size.getY(), size.getZ()),
                          216, offset, rotation);
}

string CubeMesh::toString(string prefix) {
    string s = prefix + "CubeMesh " + name + " {\n";
    s += prefix + "\toffset: " + offset.toString() + "\n";
    s += prefix + "\tcollider: " + to_string(is_collider) + "\n";
    s += prefix + "\trender: " + to_string(do_render) + "\n";
    s += prefix + "\tsize: " + size.toString() + "\n";
    s += prefix + "}";
    return s;
}

void CubeMesh::parse(ptree &tree) {
    Vector size;
    auto size_node = tree.get_child_optional(KEYWORD_SIZE);
    if (size_node.is_initialized())
        size = Vector::parse(size_node.get());
    this->size = size;
}
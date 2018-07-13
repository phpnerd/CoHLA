/*
 * Copyright (c) Tim Broenink and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

//
// Created by thomas on 8-1-18.
//

#include "Mesh.h"
#include "STLMesh.h"
#include "CubeMesh.h"

Mesh::Mesh() :
        offset(0, 0, 0),
        rotation(0, 0, 0),
        is_collider(true),
        do_render(true) {}

Mesh::Mesh(string &name, Vector &offset, Vector &rotation, bool is_collider, bool do_render) :
        name(name),
        offset(offset),
        rotation(rotation),
        is_collider(is_collider),
        do_render(do_render) {}

Mesh *Mesh::parse(string &name, ptree &tree) {
    Vector offset, rotation;
    bool is_collider = true, do_render = true;

    auto offset_node = tree.get_child_optional(KEYWORD_OFFSET);
    if (offset_node.is_initialized())
        offset = Vector::parse(offset_node.get());

    auto rotation_node = tree.get_child_optional(KEYWORD_ROTATION);
    if (rotation_node.is_initialized())
        rotation = Vector::parse(rotation_node.get());

    auto collider_node = tree.get_child_optional(KEYWORD_COLLIDER);
    if (collider_node.is_initialized())
        is_collider = collider_node.get().get_value<bool>();

    auto render_node = tree.get_child_optional(KEYWORD_RENDER);
    if (render_node.is_initialized())
        do_render = render_node.get().get_value<bool>();

    auto type_node = tree.get_child_optional(KEYWORD_MESHTYPE);
    if (!type_node.is_initialized()) {
        wcerr << "Mesh type is missing for " << name.c_str() << "!" << endl;
        return nullptr;
    }
    string type = type_node.get().get_value<string>();
    if (type == MESH_TYPE_STL)
        return new STLMesh(name, offset, rotation, is_collider, do_render, tree);
    if (type == MESH_TYPE_CUBE)
        return new CubeMesh(name, offset, rotation, is_collider, do_render, tree);
}

btTransform Mesh::getTransform() {
    btTransform transform;
    transform.setIdentity();
    transform.setOrigin(offset.toVector3());
    btQuaternion rot;
    rot.setEuler(rotation.getX(), rotation.getY(), rotation.getZ());
    transform.setRotation(rot);
    return transform;
}

string Mesh::getName() {
    return name;
}

Vector Mesh::getOffset() {
    return offset;
}

bool Mesh::isCollider() {
    return is_collider;
}

bool Mesh::doRender() {
    return do_render;
}

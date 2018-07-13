/*
 * Copyright (c) Tim Broenink and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

//
// Created by thomas on 9-1-18.
//

#include "STLMesh.h"

STLMesh::STLMesh(string &name, Vector &offset, Vector &rotation, bool is_collider, bool do_render, ptree &tree) :
        Mesh(name, offset, rotation, is_collider, do_render) {
    parse(tree);
}

btCollisionShape *STLMesh::getCollisionShape() {
    if (!file_exists(filepath)) {
        wcerr << "The file \"" << filepath.c_str() << "\" could not be read. Make sure the file exists!" << endl;
        wcerr.flush();
        return nullptr;
    }
    auto mesh = LoadColliderFromSTL(filepath.c_str());
    stl_offset = Vector(mesh.second[0], mesh.second[1], mesh.second[2]);
    return new btConvexTriangleMeshShape(mesh.first, true);
}

Renderable *STLMesh::getRenderable(Vector &color, CollisionBody *collisionBody) {
    if (!file_exists(filepath)) {
        wcerr << "The file \"" << filepath.c_str() << "\" could not be read. Make sure the file exists!" << endl;
        wcerr.flush();
        return nullptr;
    }
    vector<float> meshVector = LoadRenderFromSTL(filepath.c_str());
    unsigned int mesh_id = Renderable::meshVector(meshVector);
    return new RenderBody(color, collisionBody, mesh_id, meshVector.size(), offset, rotation);
}


string STLMesh::toString(string prefix) {
    string s = prefix + "STLMesh " + name + " {\n";
    s += prefix + "\toffset: " + offset.toString() + "\n";
    s += prefix + "\trotation: " + rotation.toString() + "\n";
    s += prefix + "\tcollider: " + to_string(is_collider) + "\n";
    s += prefix + "\trender: " + to_string(do_render) + "\n";
    s += prefix + "\tfile: " + filepath + "\n";
    s += prefix + "}";
    return s;
}

btTransform STLMesh::getTransform() {
    btTransform innertransform;
    innertransform.setIdentity();
    innertransform.setOrigin(stl_offset.toVector3());
    btTransform transform;
    transform.setIdentity();
    transform.setOrigin(offset.toVector3());
    btQuaternion rot;
    rot.setEuler(rotation.getX(), rotation.getY(), rotation.getZ());
    transform.setRotation(rot);
    return transform * innertransform;
}

void STLMesh::parse(ptree &tree) {
    string filepath;
    auto filepath_node = tree.get_child_optional(KEYWORD_FILENAME);
    if (filepath_node.is_initialized())
        filepath = filepath_node.get().get_value<string>();
    this->filepath = filepath;
}
/*
 * Copyright (c) Tim Broenink and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

//
// Created by thomas on 8-1-18.
//

#include "RigidBody.h"

RigidBody::RigidBody(string& name, map<string, Mesh*>& meshes, list<Transformation>& transformations):
    name(name),
    meshes(meshes),
    transformations(transformations)
{}

btCollisionShape* RigidBody::getCollisionShape() {
    if (!collisionShape)
        buildCollisionShape();
    return collisionShape;
}

btCollisionObject* RigidBody::getCollisionObject() {
    if (!collisionObject)
        buildCollisionObject();
    return collisionObject;
}

CollisionBody* RigidBody::getCollisionBody() {
    if (!collisionBody)
        buildCollisionBody();
    return collisionBody;
}

list<Renderable*> RigidBody::getRenderables(Vector color) {
    if (!collisionBody)
        buildCollisionBody();
    list<Renderable*> renderables;
    for (auto &mesh_pair : meshes)
        if (mesh_pair.second->doRender()) {
            Renderable* renderable = mesh_pair.second->getRenderable(color, collisionBody);
            if (renderable)
                renderables.push_back(renderable);
        }
    return renderables;
}

void RigidBody::buildCollisionShape() {
    auto collisionShape = new btCompoundShape();
    for (auto const &mesh_pair : meshes)
        if (mesh_pair.second->isCollider()) {
            btCollisionShape* subShape = mesh_pair.second->getCollisionShape();
            if (subShape)
                collisionShape->addChildShape(mesh_pair.second->getTransform(), subShape);
        }
    this->collisionShape = collisionShape;
}

void RigidBody::buildCollisionObject() {
    if (!collisionShape)
        buildCollisionShape();
    auto collisionObject = new btCollisionObject();
    collisionObject->setCollisionShape(collisionShape);
    collisionObject->setWorldTransform(btTransform::getIdentity());
    this->collisionObject = collisionObject;
}

void RigidBody::buildCollisionBody() {
    if (!collisionObject)
        buildCollisionObject();
    auto collisionBody = new CollisionBody(collisionObject);
    for (auto transformation : transformations)
        collisionBody->addTransform(transformation.getTransform());
    this->collisionBody = collisionBody;
}

string RigidBody::getName() {
    return name;
}

map<string, Mesh*> RigidBody::getMeshes() {
    return meshes;
}

list<Transformation> RigidBody::getTransformations() {
    return transformations;
}

bool RigidBody::hasTransformations() {
    return !transformations.empty();
}

RigidBody RigidBody::parse(string& name, ptree& tree) {
    map<string, Mesh*> meshes;
    list<Transformation> transformations;

    auto meshes_node = tree.get_child_optional(KEYWORD_MESHES);
    auto transformations_node = tree.get_child_optional(KEYWORD_TRANSFORMATIONS);

    if (meshes_node.is_initialized()) {
        for (ptree::value_type& mesh_node : meshes_node.get()) {
            string mesh_name = mesh_node.first;
            meshes[mesh_name] = Mesh::parse(mesh_name, mesh_node.second);
        }
    }

    if (transformations_node.is_initialized())
        for (ptree::value_type& transform_node : transformations_node.get())
            transformations.push_back(Transformation::parse(transform_node.second));

    return RigidBody(name, meshes, transformations);
}

string RigidBody::toString(string prefix) {
    string s = prefix + "RigidBody " + name + " {\n";
    s += prefix + "\tMeshes {\n";
    for (auto &mesh : meshes)
        s += mesh.second->toString(prefix + "\t\t") + "\n";
    s += prefix + "\t}\n" + prefix + "\tTransformations: {\n";
    for (auto &transformation : transformations)
        s += transformation.toString(prefix + "\t\t") + "\n";
    return s + prefix + "\t}\n" + prefix + "}";
}
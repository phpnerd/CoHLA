/*
 * Copyright (c) Tim Broenink and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

//
// Created by thomas on 8-1-18.
//

#ifndef OPENRTI_LIB_RIGIDBODY_H
#define OPENRTI_LIB_RIGIDBODY_H

#define KEYWORD_MESHES          "meshes"
#define KEYWORD_TRANSFORMATIONS "transforms"

#include <iostream>
#include <string>
#include <map>
#include <list>

#include "btBulletCollisionCommon.h"
#include "boost/property_tree/ptree.hpp"

#include "collision_body.h"
#include "renderable.h"
#include "Transformation.h"
#include "STLMesh.h"
#include "CubeMesh.h"

using namespace std;
using boost::property_tree::ptree;

class RigidBody {

public:
    RigidBody() = default;
    RigidBody(string&, map<string, Mesh*>&, list<Transformation>&);

    btCollisionShape* getCollisionShape();
    btCollisionObject* getCollisionObject();
    CollisionBody* getCollisionBody();
    list<Renderable*> getRenderables(Vector);

    void buildCollisionShape();
    void buildCollisionObject();
    void buildCollisionBody();

    string getName();
    map<string, Mesh*> getMeshes();
    list<Transformation> getTransformations();

    bool hasTransformations();

    static RigidBody parse(string&, ptree&);

    string toString(string prefix = "");

    RigidBody& operator+=(RigidBody other) {
        if (other.hasTransformations()) {
            wcerr << "Failed to add complete RigidBody to " << name.c_str() << endl;
            return *this;
        }
        for (auto &mesh_pair : other.getMeshes())
            meshes[mesh_pair.first] = mesh_pair.second;
        return *this;
    }

private:
    string name;
    map<string, Mesh*> meshes;
    list<Transformation> transformations;

    btCollisionShape* collisionShape = nullptr;
    btCollisionObject* collisionObject = nullptr;
    CollisionBody* collisionBody = nullptr;

};


#endif //OPENRTI_LIB_RIGIDBODY_H

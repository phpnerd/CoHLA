/*
 * Copyright (c) Tim Broenink and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

//
// Created by thomas on 8-1-18.
//

#ifndef OPENRTI_LIB_MESH_H
#define OPENRTI_LIB_MESH_H

#define KEYWORD_MESHTYPE    "type"
#define KEYWORD_COLLIDER    "collider"
#define KEYWORD_RENDER      "render"
#define KEYWORD_OFFSET      "offset"
#define KEYWORD_ROTATION    "rotation"

#define MESH_TYPE_STL       "stl"
#define MESH_TYPE_CUBE      "cube"

#include "boost/property_tree/ptree.hpp"
#include "btBulletCollisionCommon.h"

#include "collision_body.h"
#include "renderable.h"
#include "RenderBody.h"
#include "Vector.h"

using namespace std;
using boost::property_tree::ptree;

class Mesh {

public:
    Mesh();
    Mesh(string&, Vector&, Vector&, bool, bool);

    virtual string toString(string) = 0;
    virtual btCollisionShape* getCollisionShape() = 0;
    virtual Renderable* getRenderable(Vector&, CollisionBody*) = 0;
    virtual btTransform getTransform();

    string getName();
    virtual Vector getOffset();
    bool isCollider();
    bool doRender();

    static Mesh* parse(string&, ptree&);
    virtual void parse(ptree&) = 0;

    static string to_string(bool b) {
        return b ? "true" : "false";
    }

protected:
    string name;
    Vector offset, rotation;
    bool is_collider, do_render;

};


#endif //OPENRTI_LIB_MESH_H

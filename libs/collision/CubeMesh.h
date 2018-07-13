/*
 * Copyright (c) Tim Broenink and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

//
// Created by thomas on 9-1-18.
//

#ifndef OPENRTI_LIB_CUBEMESH_H
#define OPENRTI_LIB_CUBEMESH_H

#define KEYWORD_SIZE    "size"

#include "Mesh.h"

class CubeMesh: public Mesh {

public:
    CubeMesh(string&, Vector&, Vector&, bool, bool, ptree&);

    btCollisionShape* getCollisionShape() override;
    Renderable* getRenderable(Vector&, CollisionBody*) override;

    string toString(string) override;

    void parse(ptree&) override;

private:
    Vector size;

};


#endif //OPENRTI_LIB_CUBEMESH_H

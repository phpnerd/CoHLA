/*
 * Copyright (c) Tim Broenink and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

//
// Created by thomas on 9-1-18.
//

#ifndef OPENRTI_LIB_STLMESH_H
#define OPENRTI_LIB_STLMESH_H

#define KEYWORD_FILENAME    "file"

#include "Mesh.h"
#include "loadMeshFromSTL.h"

class STLMesh : public Mesh {

public:
    STLMesh(string &, Vector &, Vector &, bool, bool, ptree &);

    btCollisionShape *getCollisionShape() override;

    Renderable *getRenderable(Vector &, CollisionBody *) override;

    string toString(string) override;

    btTransform getTransform() override;

    void parse(ptree &) override;

private:
    string filepath;
    Vector stl_offset;

    inline bool file_exists(string &filename) {
        ifstream ifile(filename);
        return ifile ? true : false;
    }

};


#endif //OPENRTI_LIB_STLMESH_H

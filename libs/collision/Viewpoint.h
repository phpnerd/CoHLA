/*
 * Copyright (c) Tim Broenink and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

//
// Created by thomas on 8-1-18.
//

#ifndef OPENRTI_LIB_VIEWPOINT_H
#define OPENRTI_LIB_VIEWPOINT_H

#define KEYWORD_POSITION    "position"
#define KEYWORD_LOOKAT      "lookAt"

#include "boost/property_tree/ptree.hpp"

#include "Vector.h"

using boost::property_tree::ptree;

class Viewpoint {

public:
    Viewpoint();
    Viewpoint(float, float, float, float, float, float);
    Viewpoint(Vector&, Vector&);

    Vector getPosition();
    void setPosition(Vector&);
    Vector getLookAt();
    void setLookAt(Vector&);

    string toString(string prefix = "");

    static Viewpoint parse(ptree&);

private:
    Vector position, lookAt;

};


#endif //OPENRTI_LIB_VIEWPOINT_H

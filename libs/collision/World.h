/*
 * Copyright (c) Tim Broenink and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

//
// Created by thomas on 8-1-18.
//

#ifndef OPENRTI_LIB_WORLD_H
#define OPENRTI_LIB_WORLD_H

#define KEYWORD_STATES      "states"
#define KEYWORD_VIEWPOINT   "viewpoint"
#define KEYWORD_BODIES      "bodies"

#include <iostream>
#include <string>
#include <map>

#include "boost/property_tree/ptree.hpp"
#include "boost/property_tree/json_parser.hpp"

#include "Vector.h"
#include "Viewpoint.h"
#include "RigidBody.h"

using namespace std;
using boost::property_tree::ptree;

class World {

public:
    World(vector<string>&, map<string, RigidBody>&, Viewpoint viewpoint = Viewpoint());

    vector<string> getStates();
    Viewpoint getViewpoint();
    map<string, RigidBody> getBodies();

    string toString(string prefix = "");

    static World parse(string&);
    static World parseWorld(ptree&);
    static void parseBodies();

    World& operator+=(World other) {
        for (auto &body_pair : other.getBodies()) {
            if (body_pair.second.hasTransformations()) // Overwrite or add RigidBody
                bodies[body_pair.first] = body_pair.second;
            else if (bodies.find(body_pair.first) != bodies.end()) {
                bodies[body_pair.first] += body_pair.second;
            }
        }
        return *this;
    }

private:
    vector<string> states;
    Viewpoint viewpoint;
    map<string, RigidBody> bodies;

};


#endif //OPENRTI_LIB_WORLD_H

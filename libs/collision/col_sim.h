/*
 * Copyright (c) Tim Broenink and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

//
// Created by Tim on 13/06/17.
//

#ifndef COLLISION_SIM_COLSIM_H
#define COLLISION_SIM_COLSIM_H

#ifndef COLLISION_RENDER_FREQUENCY
#define COLLISION_RENDER_FREQUENCY 10
#endif // COLLISION_RENDER_FREQUENCY

#include <iostream>
#include <string>
#include <map>
#include <list>
#include <vector>
#include <chrono>
#include "btBulletCollisionCommon.h"
#include "boost/property_tree/ptree.hpp"
#include "boost/property_tree/json_parser.hpp"

#include <bullet/HACD/hacdHACD.h>

#include "loadMeshFromSTL.h"
#include "collision_body.h"
#include "renderer.h"
#include "renderable.h"
#include "RenderBody.h"
#include "World.h"


using namespace std;
using chrono::system_clock;
using boost::property_tree::ptree;

class CollisionSimulator {

public:
    CollisionSimulator(vector<string>&, bool);
    ~CollisionSimulator();
    virtual void updateAttributeValues();
    set<string> getChangedAttributes();
    void advanceTime(double);
    void setState(string, float);
    double getState(string);
    int detectCollisions();
    bool render();
    int getCollisions();
    vector<CollisionBody*> getBodies();

protected:
    set<string> changedAttributes;

    bool initialize();
    bool loadModel();

private:
    bool initialized = false;
    bool loaded = false;
    bool rendererReady = false;
    int colorcount = 0;

    vector<string> collisionModels;
    bool do_render;
    int collisions;
    map<string, float> states;
    vector<CollisionBody*> bodies;

    btDefaultCollisionConfiguration* collisionConfiguration;
    btCollisionDispatcher* dispatcher;
    btBroadphaseInterface* overlappingPairCache;
    btCollisionWorld* collisionWorld;
    btAlignedObjectArray<btCollisionShape*> collisionShapes;

    renderer* debugDraw;

    system_clock::time_point time, renderTime;
    chrono::duration<double> renderDuration;

    bool isReady();
    bool initRenderer();

    Vector getNextColor();

    Vector pallette[16] = {
            { 1.0f, 1.0f, 1.0f }, { 0.8f, 0.8f, 0.8f },
            { 0.0f, 0.0f, 1.0f }, { 0.4f, 0.4f, 1.0f },
            { 0.0f, 1.0f, 0.0f }, { 0.4f, 1.0f, 0.4f },
            { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.4f, 0.4f },
            { 0.0f, 1.0f, 1.0f }, { 0.4f, 1.0f, 1.0f },
            { 1.0f, 0.0f, 1.0f }, { 1.0f, 0.4f, 1.0f },
            { 1.0f, 1.0f, 0.0f }, { 1.0f, 1.0f, 0.4f },
            { 0.5f, 0.5f, 0.5f }, { 0.2f, 0.2f, 0.2f }
    };

    const double renderStep = 1.0 / COLLISION_RENDER_FREQUENCY;

};
#endif //COLLISION_SIM_COLSIM_H

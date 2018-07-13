/*
 * Copyright (c) Tim Broenink and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

//
// Created by Tim on 13/06/17.
//

#include "col_sim.h"

CollisionSimulator::CollisionSimulator(vector<string>& collisionModels, bool do_render):
        collisionModels(collisionModels),
        do_render(do_render),
        collisions(0)
{
    initialized = initialize();
    loaded = loadModel();
}

CollisionSimulator::~CollisionSimulator() {
    if (do_render)
        delete debugDraw;
    delete collisionWorld;
    delete collisionConfiguration;
    delete dispatcher;
    delete overlappingPairCache;
}

bool CollisionSimulator::loadModel() {
    if (!initialized)
        return false;
    if (collisionModels.empty()) {
        wcerr << "No main collision models specified" << endl;
        return false;
    }
    World world = World::parse(collisionModels[0]);
    for (int i = 1; i < collisionModels.size(); i++) {
        World _world = World::parse(collisionModels[i]);
        world += _world;
    }
    wcout << "==== BEGIN WORLD MODEL ====" << endl << world.toString().c_str() << endl << "==== END WORLD MODEL ====" << endl;
    for (auto body_pair : world.getBodies()) {
        RigidBody body = body_pair.second;
        collisionShapes.push_back(body.getCollisionShape());
        collisionWorld->addCollisionObject(body.getCollisionObject());
        if (do_render) {
            for (Renderable *renderable : body.getRenderables(getNextColor()))
                debugDraw->addRenderable(renderable);
        }
        bodies.push_back(body.getCollisionBody());
    }
    return true;
}

bool CollisionSimulator::initialize() {
    if (initialized)
        return true;
    collisionConfiguration = new btDefaultCollisionConfiguration();

    /// use the default collision dispatcher. For parallel processing you can
    /// use a diffent dispatcher (see Extras/BulletMultiThreaded)
    dispatcher = new btCollisionDispatcher(collisionConfiguration);

    /// btDbvtBroadphase is a good general purpose broadphase. You can also try
    /// out btAxis3Sweep.
    overlappingPairCache = new btDbvtBroadphase();

    collisionWorld = new btCollisionWorld(dispatcher, overlappingPairCache, collisionConfiguration);

    if (do_render)
        rendererReady = initRenderer();
    return true;
}

bool CollisionSimulator::initRenderer() {
    fprintf(stdout, "Initialising renderer\n");
    debugDraw = new renderer;
    renderTime = system_clock::now();

    collisionWorld->setDebugDrawer(debugDraw);
    return debugDraw->init() == 0;
}

int CollisionSimulator::detectCollisions() {
    if (!isReady())
        throw runtime_error("CollisionSimulator is not ready to run!");
    for (auto body : bodies)
        body->updateTransform(states);
    collisionWorld->performDiscreteCollisionDetection();
    int totalcount =0;
    float mindistance = 10.0f;
    int numManifolds = collisionWorld->getDispatcher()->getNumManifolds();
    for( int i =0; i<numManifolds; i++){
        btPersistentManifold* contactManifold =  collisionWorld->getDispatcher()->getManifoldByIndexInternal(i);
        int contacts = contactManifold->getNumContacts();
        for (int j = 0; j < contacts; j++)
        {
            btManifoldPoint& pt = contactManifold->getContactPoint(j);
            float distance = pt.getDistance();
            if (distance > mindistance){
                mindistance = distance;
            }
            if (distance < 0.f)
            {
                totalcount++;
            }
        }
    }
    return totalcount;
}

void CollisionSimulator::advanceTime(double) {
    collisions = detectCollisions();
    if (do_render)
        render();
}

bool CollisionSimulator::render() {
    if (!do_render)
        return true;
    if (!isReady())
        throw runtime_error("CollisionSimulator is not ready to render!");
    if (!rendererReady && !(rendererReady = initRenderer()))
        throw runtime_error("Failed to initialise renderer!");
    time = system_clock::now();
    renderDuration = time - renderTime;
    if (renderDuration.count() < renderStep)
        return true;
    renderTime = time;
    collisionWorld->debugDrawWorld();
    return debugDraw->render();
}

double CollisionSimulator::getState(string state) {
    return states[state];
}

void CollisionSimulator::setState(string state, float value) {
    states[state] = value;
}

int CollisionSimulator::getCollisions() {
    return collisions;
}

bool CollisionSimulator::isReady() {
    return initialized && loaded;
}

vector<CollisionBody*> CollisionSimulator::getBodies() {
    return bodies;
}

Vector CollisionSimulator::getNextColor() {
    colorcount = colorcount % 16;
    return pallette[colorcount++];
}

set<string> CollisionSimulator::getChangedAttributes() {
    set<string> ca(changedAttributes);
    changedAttributes.clear();
    return ca;
}

void CollisionSimulator::updateAttributeValues() {

}

/*
 * Copyright (c) Tim Broenink and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

//,
// Created by thomas on 20-6-17.
//

#include <iostream>
#include "col_sim.h"

using namespace std;

int main(int argc, char *argv[]) {
    if (argc < 3) {
        wcerr << "Expected at least two arguments: [collisionModel]+ [do_render (1 for true)]" << endl;
        return -1;
    }
    vector<string> collisionModels;
    for (int i = 1; i < argc - 1; i++)
        collisionModels.emplace_back(argv[i]);
    bool do_render = stoi(argv[argc - 1]) == 1;
    wcout << "Initializing Collision Simulator" << endl;

    CollisionSimulator cs(collisionModels, do_render);

    ///-----stepsimulation_start-----
    for (int i = 0; i <= 0; i += 1)
    {
        cs.setState("axis1", ((float)i));
        cs.setState("axis2", ((float)i));

        wcout << "Step: " << i << endl;
        wcout << "axis1: " << cs.getState("axis1") << ", axis2: " << cs.getState("axis2") << endl;
        wcout << "Collisions: " << cs.detectCollisions() << endl;
        cs.render();
        usleep(100000);
    }
    ///-----stepsimulation_end-----

    while(do_render && cs.render());
    return 0;
}
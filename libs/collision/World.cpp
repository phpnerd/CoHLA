/*
 * Copyright (c) Tim Broenink and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

//
// Created by thomas on 8-1-18.
//

#include "World.h"

World::World(vector<string>& states, map<string, RigidBody>& bodies, Viewpoint viewpoint):
    states(states),
    viewpoint(viewpoint),
    bodies(bodies)
{}

World World::parse(string& modelFile) {
    ptree pt;
    read_json(modelFile, pt);
    return parseWorld(pt);
}

World World::parseWorld(ptree& tree) {
    vector<string> states;
    map<string, RigidBody> bodies;

    auto bodies_node = tree.get_child_optional(KEYWORD_BODIES);
    if (bodies_node.is_initialized())
        for (ptree::value_type& body_node : bodies_node.get()) {
            string body_name = body_node.first;
            RigidBody body = RigidBody::parse(body_name, body_node.second);
            bodies[body_name] = body;
        }

    auto states_node = tree.get_child_optional(KEYWORD_STATES);
    if (!states_node.is_initialized())
        return World(states, bodies);

    for (ptree::value_type& state : states_node.get())
        states.push_back(state.second.get_value<string>());

    auto vp_node = tree.get_child_optional(KEYWORD_VIEWPOINT);
    if (!vp_node.is_initialized())
        return World(states, bodies);
    Viewpoint viewpoint = Viewpoint::parse(vp_node.get());
    return World(states, bodies, viewpoint);
}

vector<string> World::getStates() {
    return states;
}

Viewpoint World::getViewpoint() {
    return viewpoint;
}

map<string, RigidBody> World::getBodies() {
    return bodies;
}

string World::toString(string prefix) {
    string s = prefix + "World {\n";
    s += prefix + "\tStates:";
    for (auto &state : states)
        s += " \"" + state + "\"";

    s += "\n" + prefix + "\tViewpoint: " + viewpoint.toString(prefix + "\t") + "\n";

    for (auto &body : bodies)
        s += body.second.toString(prefix + "\t") + "\n";

    return s + prefix + "}";
}
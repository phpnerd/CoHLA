/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

//
// Created by thomas on 10-11-17.
//

#include "Event.h"
#include "AssignEvent.h"
#include "SocketEvent.h"

Event::Event(long double time):
    time(time),
    handled(false)
{}

Event* Event::parse(string& federateName, ifstream& infile, void* federate, map<string, Socket*>& sockets) {
    string time, type;
    getline(infile, time, ':');
    getline(infile, type, ':');
    double t = stod(time);
    if (type == EVENT_TYPE_ASSIGN)
        return AssignEvent::parse(infile, t, federate);
    if (type == EVENT_TYPE_SOCKET)
        return SocketEvent::parse(infile, t, sockets);
}

void Event::process(long double time) {
    if (doApply(time)) {
        this->handle();
        handled = true;
    }
}

long double Event::getTime() {
    return time;
}

bool Event::doApply(long double time) {
    return !handled && this->time <= time;
}
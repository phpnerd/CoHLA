/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

//
// Created by thomas on 10-11-17.
//

#ifndef OPENRTI_LIB_SCENARIO_H
#define OPENRTI_LIB_SCENARIO_H

#include <cstring>
#include <fstream>
#include <list>
#include <socket.h>
#include <boost/thread.hpp>
#include "scenario/AssignEvent.h"
#include "scenario/SocketEvent.h"
#include "Utils.h"

using namespace std;

class Scenario {

public:
    Scenario(string&, string&, void*);
    ~Scenario() = default;

    void apply(long double);

    bool hasAutoStop();
    double getAutoStopTime();

private:
    bool autoStop = false;
    double autoStopTime = 0.0;
    list<Event*> events;
    map<string, Socket*> sockets;
    map<string, boost::thread*> socketThreads;

    void parse(string&, ifstream&, void*);
    void parseSocket(string&, ifstream&);

};


#endif //OPENRTI_LIB_SCENARIO_H

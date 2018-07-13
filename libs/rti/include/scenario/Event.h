/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

//
// Created by thomas on 10-11-17.
//

#ifndef OPENRTI_LIB_EVENT_H
#define OPENRTI_LIB_EVENT_H

#define EVENT_TYPE_ASSIGN   "ASSIGN"
#define EVENT_TYPE_SOCKET   "SOCKET"

#include <cstring>
#include <fstream>
#include <list>
#include <map>
#include "socket.h"

using namespace std;

class Event {

public:
    explicit Event(long double);
    ~Event() = default;

    long double getTime();
    void process(long double);

    virtual bool forFederate(string&) = 0;

    static Event* parse(string&, ifstream&, void*, map<string, Socket*>&);

protected:
    bool doApply(long double);
    virtual void handle() = 0;

private:
    long double time;
    bool handled;

};


#endif //OPENRTI_LIB_EVENT_H

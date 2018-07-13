/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

//
// Created by thomas on 16-11-17.
//

#ifndef OPENRTI_LIB_TOPOLOGY_H
#define OPENRTI_LIB_TOPOLOGY_H

#include <iostream>
#include "Connection.h"
#include "FederateInfo.h"

using namespace std;

class Topology {

public:
    Topology() = default;
    explicit Topology(string&);

    string getName();
    string getClass(string);
    bool isFederateWatcher(string);
    list<Connection>& getConnections();
    list<FederateInfo> getFederates();

    void addConnection(Connection);
    bool hasConnection(Connection);

private:
    string name;
    map<string, FederateInfo> federates;
    list<Connection> connections;

    void parse(ifstream&);

};


#endif //OPENRTI_LIB_TOPOLOGY_H

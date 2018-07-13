/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

//
// Created by thomas on 16-11-17.
//

#include "Topology.h"

Topology::Topology(string& topologyFile) {
    ifstream infile(topologyFile);
    parse(infile);
    infile.close();
}

string Topology::getName() {
    return name;
}

list<Connection>& Topology::getConnections() {
    return connections;
}

list<FederateInfo> Topology::getFederates() {
    return values(federates);
}

string Topology::getClass(string federateName) {
    return federates[federateName].getClass();
}

bool Topology::isFederateWatcher(string federateName) {
    return federates[federateName].isWatcher();
}

void Topology::addConnection(Connection connection) {
    connections.push_back(connection);
}

bool Topology::hasConnection(Connection connection) {
    return exists(connections, connection);
}

void Topology::parse(ifstream& infile) {
    auto nameInfo = parseKeyValue(infile);
    if (nameInfo.first != "name") {
        wcerr << "Failed to parse federation name!" << endl;
        return;
    }
    name = nameInfo.second;

    auto federatesInfo = parseKeyValue(infile);
    if (federatesInfo.first != "federates") {
        wcerr << "Failed to parse federates information!" << endl;
        return;
    }
    int nrOfFederates = stoi(federatesInfo.second);
    for (int i = 0; i < nrOfFederates; i++) {
        string instanceName, instanceClass, _instanceWatcher;
        getline(infile, instanceName, ':');
        getline(infile, instanceClass, ':');
        getline(infile, _instanceWatcher);
        bool instanceWatcher = _instanceWatcher == "true";
        federates[instanceName] = FederateInfo(instanceName, instanceClass, instanceWatcher);
    }
    auto connectionsInfo = parseKeyValue(infile);
    if (connectionsInfo.first != "connections") {
        wcerr << "Failed to parse connections information!" << endl;
        return;
    }
    int nrOfConnections = stoi(connectionsInfo.second);
    for (int i = 0; i < nrOfConnections; i++)
        connections.emplace_back(Connection::parse(infile));
}
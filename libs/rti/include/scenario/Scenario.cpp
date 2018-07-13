/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

//
// Created by thomas on 10-11-17.
//

#include "Scenario.h"

Scenario::Scenario(string& federateName, string& scenarioFile, void* federate) {
    ifstream infile(scenarioFile);
    parse(federateName, infile, federate);
    infile.close();
}

void Scenario::apply(long double time) {
    for (auto &event : events)
        event->process(time);
}

void Scenario::parse(string& federateName, ifstream& infile, void* federate) {
    auto autoStopInfo = parseKeyValue(infile);
    if (autoStopInfo.first != "autostop") {
        wcerr << "Could not find keyword \"autostop\" in Scenario!" << endl;
        return;
    }
    autoStopTime = stod(autoStopInfo.second);
    if (autoStopTime > 0.0)
        autoStop = true;
    auto socketInfo = parseKeyValue(infile);
    if (socketInfo.first != "sockets") {
        wcerr << "Could not find keyword \"sockets\" in Scenario!" << endl;
        return;
    }
    int nrOfSockets = stoi(socketInfo.second);
    for (int i = 0; i < nrOfSockets; i++)
        parseSocket(federateName, infile);
    auto eventInfo = parseKeyValue(infile);
    if (eventInfo.first != "events") {
        wcerr << "Could not find keyword \"events\" in Scenario!" << endl;
        return;
    }
    int nrOfEvents = stoi(eventInfo.second);
    for (int i = 0; i < nrOfEvents; i++) {
        Event* event = Event::parse(federateName, infile, federate, sockets);
        if (event->forFederate(federateName))
            events.emplace_back(event);
    }
}

void Scenario::parseSocket(string& federateName, ifstream& infile) {
    string fName, socketName, hostname, port;
    getline(infile, fName, '.');
    getline(infile, socketName, ':');
    getline(infile, hostname, ':');
    getline(infile, port);
    if (fName == federateName) {
        sockets[socketName] = new Socket(hostname, (uint16_t) stoi(port));
        socketThreads[socketName] = new boost::thread(&Socket::connect, sockets[socketName], true);
    }
}

bool Scenario::hasAutoStop() {
    return autoStop;
}

double Scenario::getAutoStopTime() {
    return autoStopTime;
}
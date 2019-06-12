/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#include "PooslSimulator.h"


PooslSimulator::PooslSimulator(string& hostname, uint16_t port):
        hostname(hostname),
        port(port),
        socket(new Socket(hostname, port))
{
    connect();
}

void PooslSimulator::connect() {
    wcout << "Connecting to POOSL model on " << hostname.c_str() << ":" << port << endl;
    socket->connect(false);
    wcout << "Connected to POOSL model" << endl;
}

void PooslSimulator::write(PooslMessage& msg) {
    socket->write(msg.serialize());
}

PooslMessage* PooslSimulator::awaitMessage(function<bool(PooslMessage*)> filter) {
    PooslMessage* msg = nextMessage();
    while (!filter(msg)) {
        msgQueue.push(msg);
        msg = nextMessage();
    }
    return msg;
}

PooslMessage* PooslSimulator::nextMessage() {
    string s = socket->readLine();
    PooslMessage* msg = PooslMessage::parse(s);
    return msg;
}

long double PooslSimulator::waitReady() {
    PooslMessage* msg;
    while (!msgQueue.empty()) {
        msg = msgQueue.front();
        msgQueue.pop();
        if (!msg->isTimeMessage() || !((PooslTimeMessage*)msg)->isRequest())
            handle(msg);
        else
            return ((PooslTimeMessage*)msg)->getStep();
    }
    msg = nextMessage();
    while (!msg->isTimeMessage() || !((PooslTimeMessage*)msg)->isRequest()) {
        handle(msg);
        msg = nextMessage();
    }
    return ((PooslTimeMessage*)msg)->getStep();

}

void PooslSimulator::advanceTime(long double time) {
    auto msg = PooslTimeMessage::makeGrant(time);
    write(msg);
}

set<string> PooslSimulator::getChangedAttributes() {
    return changedAttributes;
}

void PooslSimulator::updateAttributeValues(string process) {
    auto msg = PooslAttributeMessage::makeRequest(process);
    write(msg);
    PooslAttributeMessage* resp = (PooslAttributeMessage*)awaitMessage([](PooslMessage* _msg) {
        return _msg->isAttributeMessage();
    });
    attrCache[process] = resp->getValues();
}

bool PooslSimulator::setAttribute(std::string& process, std::string& name, int value) {
    string val = to_string(value);
    return setAttribute(process, name, val);
}

bool PooslSimulator::setAttribute(std::string& process, std::string& name, double value) {
    string val = to_string(value);
    return setAttribute(process, name, val);
}

bool PooslSimulator::setAttribute(std::string& process, std::string& name, long value) {
    string val = to_string(value);
    return setAttribute(process, name, val);
}

bool PooslSimulator::setAttribute(std::string& process, std::string& name, bool value) {
    string val = value ? "true" : "false";
    return setAttribute(process, name, val);
}

bool PooslSimulator::setAttribute(std::string& process, std::string& name, string& value) {
    auto msg = PooslAttributeMessage(process, name, value);
    write(msg);
    PooslAttributeMessage* resp = (PooslAttributeMessage*)awaitMessage([](PooslMessage* _msg) {
        return _msg->isAttributeMessage();
    });
    return resp->getValue() == value;
}

int PooslSimulator::getInt(std::string& process, std::string& name) {
    return stoi(getString(process, name));
}

double PooslSimulator::getDouble(std::string& process, std::string& name) {
    return stod(getString(process, name));
}

long PooslSimulator::getLong(std::string& process, std::string& name) {
    return stol(getString(process, name));
}

bool PooslSimulator::getBool(std::string& process, std::string& name) {
    string val = getString(process, name);
    return val == "true" || val == "TRUE" || val == "True";
}

string PooslSimulator::getString(std::string& process, std::string& name) {
    auto msg = PooslAttributeMessage::makeRequest(process, name);
    write(msg);
    PooslAttributeMessage* resp = (PooslAttributeMessage*)awaitMessage([](PooslMessage* _msg) {
        return _msg->isAttributeMessage();
    });
    return resp->getValue();
}


void PooslSimulator::handle(PooslMessage*) {}
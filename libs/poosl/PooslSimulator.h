/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#ifndef OPENRTI_LIB_POOSLSIMULATOR_H
#define OPENRTI_LIB_POOSLSIMULATOR_H


#include <string>

#include <socket.h>
#include <set>
#include <queue>

#include "PooslMessage.h"
#include "PooslTimeMessage.h"
#include "PooslAttributeMessage.h"

class PooslSimulator {

public:
    PooslSimulator(std::string&, uint16_t);
    long double waitReady();
    void advanceTime(long double);
    void updateAttributeValues(std::string);
    virtual void updateAttributeValues() = 0;
    set<string> getChangedAttributes();

    bool setAttribute(std::string&, std::string&, int);
    bool setAttribute(std::string&, std::string&, double);
    bool setAttribute(std::string&, std::string&, long);
    bool setAttribute(std::string&, std::string&, bool);
    bool setAttribute(std::string&, std::string&, std::string&);
    int getInt(std::string&, std::string&);
    double getDouble(std::string&, std::string&);
    long getLong(std::string&, std::string&);
    bool getBool(std::string&, std::string&);
    string getString(std::string&, std::string&);

protected:
    set<string> changedAttributes;
    std::map<std::string, std::map<std::string, std::string>> attrCache;

    void connect();
    virtual void handle(PooslMessage*);

private:
    std::string hostname;
    uint16_t port;

    Socket* socket;
    queue<PooslMessage*> msgQueue;

    void write(PooslMessage&);
    PooslMessage* awaitMessage(function<bool(PooslMessage*)>);
    PooslMessage* nextMessage();

};


#endif //OPENRTI_LIB_POOSLSIMULATOR_H

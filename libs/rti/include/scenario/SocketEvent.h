/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

//
// Created by thomas on 10-11-17.
//

#ifndef OPENRTI_LIB_SOCKETEVENT_H
#define OPENRTI_LIB_SOCKETEVENT_H

#include "Event.h"

class SocketEvent: public Event {

public:
    SocketEvent(long double, string&, Socket*);

    bool forFederate(string&) override;

    static Event* parse(ifstream&, long double, map<string, Socket*>&);

protected:
    void handle() override;

private:
    string bytes;
    Socket* socket;

};


#endif //OPENRTI_LIB_SOCKETEVENT_H

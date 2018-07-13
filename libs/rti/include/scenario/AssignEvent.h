/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

//
// Created by thomas on 10-11-17.
//

#ifndef OPENRTI_LIB_ASSIGNEVENT_H
#define OPENRTI_LIB_ASSIGNEVENT_H

#include <models/FederateAttributeReference.h>
#include "Event.h"

class AssignEvent: public Event {

public:
    AssignEvent(long double, FederateAttributeReference&, string&, void*);

    bool forFederate(string&) override;

    static Event* parse(ifstream&, long double, void*);

protected:
    void handle() override;

private:
    FederateAttributeReference ref;
    string value;
    void* fed;

};


#endif //OPENRTI_LIB_ASSIGNEVENT_H

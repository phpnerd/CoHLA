/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

//
// Created by thomas on 10-11-17.
//

#include <iostream>
#include "federate.h"
#include "AssignEvent.h"

AssignEvent::AssignEvent(long double time, FederateAttributeReference& ref, string& value, void* fed):
    ref(ref),
    value(value),
    fed(fed),
    Event::Event(time)
{}

bool AssignEvent::forFederate(string& federateName) {
    return ref.getInstance() == federateName;
}

Event* AssignEvent::parse(ifstream& infile, long double time, void* fed) {
    FederateAttributeReference ref = FederateAttributeReference::parse(infile);
    string value;
    getline(infile, value);
    return new AssignEvent(time, ref, value, fed);
}

void AssignEvent::handle() {
    ((federate*)fed)->setAttribute(ref.getAttribute(), value);
}
/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

//
// Created by thomas on 14-11-17.
//

#include "ConnectionFault.h"

ConnectionFault::ConnectionFault(FederateAttributeReference reference, string& type, TimeRange time):
    TimedFault(reference, type, time)
{}

Fault* ConnectionFault::parse(ifstream& infile, FederateAttributeReference reference, string& type) {
    TimeRange time = TimeRange::parse(infile, ':', '\n');
    return new ConnectionFault(reference, type, time);
}

VariableLengthData ConnectionFault::apply(VariableLengthData data) {
    applied = true;
    data.setDataPointer(nullptr, 0);
    return data;
}
/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

//
// Created by thomas on 14-11-17.
//

#include "AbsoluteFault.h"

AbsoluteFault::AbsoluteFault(FederateAttributeReference reference, string& type, TimeRange time, string &value):
    value(value),
    TimedFault::TimedFault(reference, type, time)
{}

Fault* AbsoluteFault::parse(ifstream& infile, FederateAttributeReference reference, string& type) {
    TimeRange time = TimeRange::parse(infile, ':');
    string value;
    getline(infile, value);
    return new AbsoluteFault(reference, type, time, value);
}

VariableLengthData AbsoluteFault::apply(VariableLengthData data) {
    applied = true;
    return sToVLD(value, type);
}
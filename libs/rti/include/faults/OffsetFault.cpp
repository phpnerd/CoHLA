/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

//
// Created by thomas on 14-11-17.
//

#include "OffsetFault.h"

OffsetFault::OffsetFault(FederateAttributeReference reference, string& type, TimeRange time, string& value):
    value(value),
    TimedFault::TimedFault(reference, type, time)
{}

Fault* OffsetFault::parse(ifstream& infile, FederateAttributeReference reference, string& type) {
    TimeRange time = TimeRange::parse(infile, ':');
    string value;
    getline(infile, value);
    return new OffsetFault(reference, type, time, value);
}

VariableLengthData OffsetFault::apply(VariableLengthData data) {
    applied = true;
    if (type == TYPE_INT)
        return toVLD<int>(toType<int>(data) + fromString<int>(value));
    if (type == TYPE_LONG)
        return toVLD<long>(toType<long>(data) + fromString<long>(value));
    if (type == TYPE_DOUBLE)
        return toVLD<double>(toType<double>(data) + fromString<double>(value));
    return data;
}
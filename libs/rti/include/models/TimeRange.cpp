/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

//
// Created by thomas on 14-11-17.
//

#include "TimeRange.h"

TimeRange::TimeRange(long double from, long double to):
    from(from),
    to(to <= from ? from : to)
{}

bool TimeRange::before(long double time) {
    return time >= from;
}

bool TimeRange::inRange(long double time) {
    return time >= from && time <= to;
}

TimeRange TimeRange::parse(ifstream& infile, char splitChar, char endChar) {
    string _from, _to;
    getline(infile, _from, splitChar);
    getline(infile, _to, endChar);
    if (_to == "-")
        return TimeRange(stod(_from));
    return TimeRange(stod(_from), stod(_to));
}
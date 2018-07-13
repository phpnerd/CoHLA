/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

//
// Created by thomas on 14-11-17.
//

#ifndef OPENRTI_LIB_TIMERANGE_H
#define OPENRTI_LIB_TIMERANGE_H

#include <fstream>
#include <limits>
#include <string>

using namespace std;

class TimeRange {

public:
    explicit TimeRange(long double from, long double to = numeric_limits<long double>::max());
    ~TimeRange() = default;

    bool before(long double);
    bool inRange(long double);

    static TimeRange parse(ifstream&, char splitChar = '-', char endChar=':');

private:
    long double from, to;

};


#endif //OPENRTI_LIB_TIMERANGE_H

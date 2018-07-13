/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

//
// Created by thomas on 7-11-17.
//

#ifndef OPENRTI_LIB_METRICBASE_H
#define OPENRTI_LIB_METRICBASE_H

#include <limits>
#include <string>
#include <iostream>
#include <fstream>
#include <cmath>
#include <map>
#include <list>
#include "Utils.h"


using namespace std;

class MetricBase {

public:
    MetricBase(const string&, pair<string, string>, string, bool = false, pair<string, string> = pair<string, string>());
    ~MetricBase() = default;

    string getName();
    virtual void addData(const string&, long double, VariableLengthData) = 0;
    bool hasReference();
    string getAttributeName();
    string getRefAttributeName();
    string getInstance();
    string getAttribute();
    string getRefInstance();
    string getRefAttribute();
    string getType();
    bool isFinished();

    static MetricBase* parse(ifstream&);
    static MetricBase* parse(string, ifstream&);

    virtual string getResultLine() = 0;

protected:
    bool hasRefAttr, finished;
    string name, attributeName, refAttributeName, type;
    pair<string, string> attribute, refAttribute;

    bool endMetric, ending;
    long double endTime, endMetricDelay;

};


#endif //OPENRTI_LIB_METRICBASE_H

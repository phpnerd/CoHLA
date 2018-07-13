/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

//
// Created by thomas on 2-11-17.
//

#ifndef OPENRTI_LIB_METRIC_H
#define OPENRTI_LIB_METRIC_H

#include "MetricBase.h"

#define METRIC_ENDVALUE "EV"
#define METRIC_ERROR    "ERR"
#define METRIC_TIMER    "TIMER"
#define METRIC_MIN      "MIN"
#define METRIC_MAX      "MAX"

#define METRIC_OPTION_NONE      "NONE"
#define METRIC_OPTION_ABSOLUTE  "ABSOLUTE"
#define METRIC_OPTION_SQUARED   "SQUARED"

#define METRIC_REL_OTHER        "REL_OTHER"
#define METRIC_REL_SELF         "REL_SELF"

template<typename T>
class Metric: public MetricBase {

public:
    Metric(const string&, pair<string, string>, string&, bool hasRef = false, pair<string, string> refAttribute = pair<string, string>());

    void addData(const string&, long double, T);
    void addData(const string&, long double, VariableLengthData) override;

    static Metric<T>* parse(ifstream&, string&);

    string getResultLine() override;

protected:
    map<string, map<double, T>> data;
    map<double, T> getAttributeData();
    map<double, T> getRefAttributeData();
    virtual T compute() = 0;
    virtual void update(const string&, long double, T);

};


#endif //OPENRTI_LIB_METRIC_H

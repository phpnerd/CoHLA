/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

//
// Created by thomas on 7-11-17.
//

#ifndef OPENRTI_LIB_TIMERMETRIC_H
#define OPENRTI_LIB_TIMERMETRIC_H

#include "Metric.h"

template <typename T>
class TimerMetric: Metric<T> {

public:
    TimerMetric(const string&, pair<string, string>, string&, T, string&, bool endMetric = false, double endMetricDelay = 0.0);

    static TimerMetric<T>* parse(string&, string&, ifstream&);

    string getResultLine() override;

protected:
    T compute();
    void update(const string&, long double, T) override;

private:
    string comparator;
    T compareValue;

    double getTime();

};

#endif //OPENRTI_LIB_TIMERMETRIC_H
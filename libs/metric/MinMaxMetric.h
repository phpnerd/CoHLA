/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

//
// Created by thomas on 28-11-17.
//

#ifndef OPENRTI_LIB_MINMAXMETRIC_H
#define OPENRTI_LIB_MINMAXMETRIC_H

#include "Metric.h"

template<typename T>
class MinMaxMetric : public Metric<T> {

public:
    MinMaxMetric(const string&, bool, pair<string, string>, string&);

    static Metric<T>* parse(string&, string&, ifstream&, bool);

protected:
    T compute();

private:
    bool min;

};


#endif //OPENRTI_LIB_MINMAXMETRIC_H

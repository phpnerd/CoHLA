/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

//
// Created by thomas on 28-11-17.
//

#include "MinMaxMetric.h"

template<typename T>
MinMaxMetric<T>::MinMaxMetric(const string &name, bool isMin, pair<string, string> attribute, string& type):
        min(isMin),
        Metric<T>::Metric(name, attribute, type)
{}

template<typename T>
T MinMaxMetric<T>::compute() {
    map<double, T> d = this->getAttributeData();
    if (d.empty())
        return T();
    T result = d[0];
    for (T v : values(d))
        if (min ? v < result : v > result)
            result = v;
    return result;
}

template<>
string MinMaxMetric<string>::compute() {
    return "";
}

template<typename T>
Metric<T>* MinMaxMetric<T>::parse(string& name, string& typeName, ifstream &infile, bool isMin) {
    pair<string, string> attr;
    parseInstanceAttribute(infile, attr, true);
    return new MinMaxMetric(name, isMin, attr, typeName);
}

template class MinMaxMetric<bool>;
template class MinMaxMetric<int>;
template class MinMaxMetric<long>;
template class MinMaxMetric<double>;
template class MinMaxMetric<string>;
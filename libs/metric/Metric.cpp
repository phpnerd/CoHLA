/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

//
// Created by thomas on 2-11-17.
//

#include "Metric.h"
#include "EndValueMetric.h"
#include "ErrorMetric.h"
#include "TimerMetric.h"
#include "MinMaxMetric.h"

template<typename T>
Metric<T>::Metric(const string &name, pair<string, string> attribute, string& type, bool hasRef, pair<string, string> refAttribute):
        MetricBase(name, attribute, type, hasRef, refAttribute)
{}

template <typename T>
void Metric<T>::addData(const string& attributeName, long double time, T value) {
    if (attributeName == this->attributeName) {
        data[attributeName][time] = value;
        update(attributeName, time, value);
    }
    if (hasRefAttr && attributeName == refAttributeName) {
        this->data[attributeName][time] = value;
        update(attributeName, time, value);
    }
}

template <typename T>
void Metric<T>::addData(const string& attributeName, long double time, VariableLengthData data) {
    addData(attributeName, time, toType<T>(data));
}

template <typename T>
void Metric<T>::update(const string&, long double, T) {}

template <typename T>
Metric<T>* Metric<T>::parse(ifstream& infile, string& typeName) {
    string type, name;
    getline(infile, type, ':');
    getline(infile, name, ':');
    if (type == METRIC_ENDVALUE)
        return (Metric*)EndValueMetric<T>::parse(name, typeName, infile);
    if (type == METRIC_ERROR)
        return (Metric*)ErrorMetric<T>::parse(name, typeName, infile);
    if (type == METRIC_TIMER)
        return (Metric*)TimerMetric<T>::parse(name, typeName, infile);
    if (type == METRIC_MIN || type == METRIC_MAX)
        return (Metric*)MinMaxMetric<T>::parse(name, typeName, infile, type == METRIC_MIN);
}

template <typename T>
string Metric<T>::getResultLine() {
    return name + ": " + toString<T>(compute());
}

template <typename T>
map<double, T> Metric<T>::getAttributeData() {
    return data[attributeName];
};

template <typename T>
map<double, T> Metric<T>::getRefAttributeData() {
    return data[refAttributeName];
}

template class Metric<bool>;
template class Metric<int>;
template class Metric<long>;
template class Metric<double>;
template class Metric<string>;
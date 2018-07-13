/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

//
// Created by thomas on 7-11-17.
//

#include "TimerMetric.h"

template <typename T>
TimerMetric<T>::TimerMetric(const string& name, pair<string, string> attribute, string& comparator, T value, string& type, bool endMetric, double endMetricDelay):
        comparator(comparator),
        compareValue(value),
        Metric<T>::Metric(name, attribute, type, false)
{
    this->endMetric = endMetric;
    this->endMetricDelay = endMetricDelay;
}

template <typename T>
T TimerMetric<T>::compute() {
    return T();
}

template <typename T>
void TimerMetric<T>::update(const string& name, long double time, T value) {
    if (this->endMetric) {
        if ((!(this->ending)) && compare<T>(value, compareValue, comparator)) {
            this->ending = true;
            this->endTime = time + this->endMetricDelay;
        } else if (this->ending && time >= this->endTime)
            this->finished = true;
    }
}

template <typename T>
double TimerMetric<T>::getTime() {
    for (auto const &value : this->getAttributeData())
        if (compare<T>(value.second, compareValue, comparator))
            return value.first;
    return -1.0;
}

template<typename T>
TimerMetric<T>* TimerMetric<T>::parse(string& name, string& typeName, ifstream &infile) {
    string comp, value, _endMetric, _endMetricDelay;
    pair<string, string> attr;
    parseInstanceAttribute(infile, attr, false);
    getline(infile, comp, ':');
    getline(infile, value, ':');
    getline(infile, _endMetric, ':');
    getline(infile, _endMetricDelay);
    bool endMetric = fromString<bool>(_endMetric);
    double endMetricDelay = fromString<double>(_endMetricDelay);
    if (!endMetric)
        return new TimerMetric<T>(name, attr, comp, fromString<T>(value), typeName);
    else
        return new TimerMetric<T>(name, attr, comp, fromString<T>(value), typeName, endMetric, endMetricDelay);
}

template<typename T>
string TimerMetric<T>::getResultLine() {
    return this->name + ": " + to_string(getTime());
}

template class TimerMetric<bool>;
template class TimerMetric<int>;
template class TimerMetric<long>;
template class TimerMetric<double>;
template class TimerMetric<string>;
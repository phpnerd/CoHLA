/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

//
// Created by thomas on 3-11-17.
//

#include "ErrorMetric.h"

template<typename T>
ErrorMetric<T>::ErrorMetric(const string &name, bool squared, pair<string, string> attribute, pair<string, string> refAttribute, string& type):
        squared(squared),
        Metric<T>::Metric(name, attribute, type, true, refAttribute)
{}

template<typename T>
T ErrorMetric<T>::compute() {
    list<pair<double, T>> error;
    auto refIt = this->getRefAttributeData().begin();
    double pTime = 0.0;
    for (auto const &value : this->getAttributeData()) {
        pair<double, T> err;
        if (value.first > refIt->first) {
            err.first = refIt->first - pTime;
            err.second = value.second - refIt->second;
            pTime = refIt->first;
            error.push_back(err);
            refIt++;
        }
        if (value.first == refIt->first)
            continue;
        err.first = value.first - pTime;
        err.second = value.second - refIt->second;
        pTime = value.first;
        error.push_back(err);
    }
    double totalTime = 0.0, errorSum = 0.0;
    for (auto const &e : error) {
        totalTime += e.first;
        errorSum += e.first * (squared ? e.second * e.second : e.second);
    }
    return errorSum / totalTime;
}

template<>
string ErrorMetric<string>::compute() {
    return "";
}

template<typename T>
Metric<T>* ErrorMetric<T>::parse(string& name, string& typeName, ifstream &infile) {
    string option;
    getline(infile, option, ':');
    bool squared = option == METRIC_OPTION_SQUARED;
    pair<string, string> attr, refAttr;
    parseInstanceAttribute(infile, attr, false);
    parseInstanceAttribute(infile, refAttr, true);
    return new ErrorMetric(name, squared, attr, refAttr, typeName);
}

template class ErrorMetric<bool>;
template class ErrorMetric<int>;
template class ErrorMetric<long>;
template class ErrorMetric<double>;
template class ErrorMetric<string>;
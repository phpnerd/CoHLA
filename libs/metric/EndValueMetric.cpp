/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

//
// Created by thomas on 2-11-17.
//

#include "EndValueMetric.h"

template<typename T>
EndValueMetric<T>::EndValueMetric(const string &name, bool absolute, pair<string, string> attribute, string& type, bool hasRef, pair<string, string> refAttribute):
    absolute(absolute),
    Metric<T>::Metric(name, attribute, type, hasRef, refAttribute)
{}

template<typename T>
T EndValueMetric<T>::compute() {
    auto dataIt = this->data[this->attributeName].rbegin();
    T ev;
    if (this->hasRefAttr) {
        auto refDataIt = this->data[this->refAttributeName].rbegin();
        ev = dataIt->second - refDataIt->second;
    } else
        ev = dataIt->second;
    return absolute ? abs(ev) : ev;
}

template<>
string EndValueMetric<string>::compute() {
    return getAttributeData().rbegin()->second;
}

template<typename T>
EndValueMetric<T>* EndValueMetric<T>::parse(string name, string& typeName, ifstream &infile) {
    string buf;
    getline(infile, buf, ':');
    bool absolute = buf == METRIC_OPTION_ABSOLUTE;
    pair<string, string> attr;
    parseInstanceAttribute(infile, attr, false);
    getline(infile, buf, ':');
    if (buf == METRIC_REL_SELF) {
        getline(infile, buf);
        return new EndValueMetric<T>(name, absolute, attr, typeName);
    }
    pair<string, string> refAttr;
    parseInstanceAttribute(infile, refAttr, true);
    return new EndValueMetric<T>(name, absolute, attr, typeName, true, refAttr);
}

template class EndValueMetric<bool>;
template class EndValueMetric<int>;
template class EndValueMetric<long>;
template class EndValueMetric<double>;
template class EndValueMetric<string>;
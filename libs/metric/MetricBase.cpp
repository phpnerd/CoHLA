/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

//
// Created by thomas on 7-11-17.
//

#include "MetricBase.h"
#include "Metric.h"

MetricBase::MetricBase(const string &name, pair<string, string> attribute, string type, bool hasRef, pair<string, string> refAttribute):
        name(name),
        hasRefAttr(hasRef),
        attributeName(attribute.first + toFirstUpper(attribute.second)),
        refAttributeName(refAttribute.first + toFirstUpper(refAttribute.second)),
        attribute(attribute),
        refAttribute(refAttribute),
        type(type),
        finished(false),
        ending(false),
        endMetric(false),
        endMetricDelay(0.0),
        endTime(0.0)
{}

MetricBase* MetricBase::parse(ifstream &infile) {
    string attrType;
    getline(infile, attrType, ':');
    if (attrType == TYPE_BOOL)
        return Metric<bool>::parse(infile, attrType);
    if (attrType == TYPE_INT)
        return Metric<int>::parse(infile, attrType);
    if (attrType == TYPE_LONG)
        return Metric<long>::parse(infile, attrType);
    if (attrType == TYPE_DOUBLE)
        return Metric<double>::parse(infile, attrType);
    if (attrType == TYPE_STRING)
        return Metric<string>::parse(infile, attrType);
    return nullptr;
}

MetricBase* MetricBase::parse(string, ifstream&) {
    return nullptr;
}

string MetricBase::getName() {
    return name;
}

bool MetricBase::hasReference() {
    return hasRefAttr;
}

string MetricBase::getAttributeName() {
    return attributeName;
}

string MetricBase::getRefAttributeName() {
    return refAttributeName;
}

string MetricBase::getInstance() {
    return attribute.first;
}

string MetricBase::getAttribute() {
    return attribute.second;
}

string MetricBase::getRefInstance() {
    return refAttribute.first;
}

string MetricBase::getRefAttribute() {
    return refAttribute.second;
}

string MetricBase::getType() {
    return type;
}

bool MetricBase::isFinished() {
    return endMetric ? finished : true;
}
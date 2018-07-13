/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

//
// Created by thomas on 9-11-17.
//

#include "SubscribedAttribute.h"
#include "FederateAttributeReference.h"

SubscribedAttribute::SubscribedAttribute(FederateAttributeReference srcRef, ObjectClassHandle objectClass, AttributeHandle attribute, string target, string type, list<Fault*>& faults):
    srcRef(srcRef),
    objectClass(objectClass),
    attribute(attribute),
    target(target),
    type(type),
    read(true)
{
    for (auto &fault : faults)
        if (fault->getReference() == srcRef)
            this->faults.push_back(fault);
}

FederateAttributeReference SubscribedAttribute::getSourceReference() {
    return srcRef;
}

ObjectClassHandle SubscribedAttribute::getSourceObject() {
    return objectClass;
}

ObjectInstanceHandle SubscribedAttribute::getSourceInstance() {
    return instance;
}

void SubscribedAttribute::setSourceInstance(ObjectInstanceHandle instance) {
    this->instance = instance;
}

AttributeHandle SubscribedAttribute::getSourceAttribute() {
    return attribute;
}

string SubscribedAttribute::getTarget() {
    return target;
}

string SubscribedAttribute::getType() {
    return type;
}

bool SubscribedAttribute::isRead() {
    return read;
}

long double SubscribedAttribute::getTime() {
    return lastUpdated;
}

VariableLengthData SubscribedAttribute::getData() {
    read = true;
    return data;
}

void SubscribedAttribute::setData(long double time, VariableLengthData data) {
    for (auto &fault : faults)
        if (fault->doApply(time))
            data = fault->apply(data);
    if (data.size() == 0)
        return;
    read = false;
    this->lastUpdated = time;
    this->data = data;
}
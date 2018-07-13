/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

//
// Created by thomas on 9-11-17.
//

#include "FederateAttributeReference.h"

FederateAttributeReference::FederateAttributeReference(string instance, string attribute):
    instance(instance),
    attribute(attribute)
{}

std::string FederateAttributeReference::getInstance() {
    return instance;
}

void FederateAttributeReference::setInstance(string& instance) {
    this->instance = instance;
}

std::string FederateAttributeReference::getAttribute() {
    return attribute;
}

void FederateAttributeReference::setAttribute(string& attribute) {
    this->attribute = attribute;
}

string FederateAttributeReference::getRefName(bool firstUpper) {
    return (firstUpper ? toFirstUpper(instance) : instance) + toFirstUpper(attribute);
}

FederateAttributeReference FederateAttributeReference::parse(ifstream& infile, bool toEnd, char endChar) {
    string i, a;
    getline(infile, i, '.');
    if (toEnd)
        getline(infile, a);
    else
        getline(infile, a, endChar);
    return FederateAttributeReference(i, a);
}

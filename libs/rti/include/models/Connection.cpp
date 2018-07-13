/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

//
// Created by thomas on 9-11-17.
//

#include "Connection.h"

Connection::Connection(FederateAttributeReference source, FederateAttributeReference target, string typeName):
    source(source),
    target(target),
    typeName(typeName)
{}

FederateAttributeReference Connection::getSource() {
    return source;
}

void Connection::setSource(FederateAttributeReference source) {
    this->source = source;
}

FederateAttributeReference Connection::getTarget() {
    return target;
}

void Connection::setTarget(FederateAttributeReference target) {
    this->target = target;
}

string Connection::getTypeName() {
    return typeName;
}

Connection Connection::parse(ifstream& infile) {
    FederateAttributeReference  target = FederateAttributeReference::parse(infile, false, '-');
    FederateAttributeReference source = FederateAttributeReference::parse(infile);
    string typeName;
    getline(infile, typeName);
    return Connection(source, target, typeName);
}

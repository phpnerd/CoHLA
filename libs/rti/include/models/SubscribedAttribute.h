/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

//
// Created by thomas on 9-11-17.
//

#ifndef OPENRTI_LIB_SUBSCRIBEDATTRIBUTE_H
#define OPENRTI_LIB_SUBSCRIBEDATTRIBUTE_H

#include <cstring>
#include <RTI/Handle.h>
#include <faults/Fault.h>
#include "FederateAttributeReference.h"

using namespace std;

class SubscribedAttribute {

public:
    SubscribedAttribute(FederateAttributeReference, ObjectClassHandle, AttributeHandle, string, string, list<Fault*>&);
    ~SubscribedAttribute() = default;

    FederateAttributeReference getSourceReference();
    ObjectClassHandle getSourceObject();
    void setSourceInstance(ObjectInstanceHandle);
    ObjectInstanceHandle getSourceInstance();
    AttributeHandle getSourceAttribute();
    string getTarget();
    string getType();

    bool isRead();
    long double getTime();
    VariableLengthData getData();
    void setData(long double, VariableLengthData);

private:
    FederateAttributeReference srcRef;
    ObjectClassHandle objectClass;
    ObjectInstanceHandle instance;
    AttributeHandle attribute;
    string target, type;
    bool read;
    long double lastUpdated;
    VariableLengthData data;

    list<Fault*> faults;

};


#endif //OPENRTI_LIB_SUBSCRIBEDATTRIBUTE_H

/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

//
// Created by thomas on 14-11-17.
//

#ifndef OPENRTI_LIB_FAULT_H
#define OPENRTI_LIB_FAULT_H

#define FAULT_TYPE_CONNECTION   "TIMEDCON"
#define FAULT_TYPE_OFFSET       "TIMEDOFF"
#define FAULT_TYPE_ABSOLUTE     "TIMEDABS"
#define FAULT_TYPE_VARIANCE     "VARIANCE"

#include <iostream>
#include <models/FederateAttributeReference.h>

using namespace std;

class Fault {

public:
    explicit Fault(FederateAttributeReference, string&);
    ~Fault() = default;

    FederateAttributeReference getReference();
    string getType();

    virtual bool doApply(long double);
    virtual VariableLengthData apply(VariableLengthData) = 0;

    static list<Fault*> parse(string&);

protected:
    static Fault* parse(ifstream&);
    FederateAttributeReference reference;
    string type;

};


#endif //OPENRTI_LIB_FAULT_H

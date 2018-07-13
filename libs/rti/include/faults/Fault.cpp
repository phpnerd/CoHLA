/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

//
// Created by thomas on 14-11-17.
//

#include "Fault.h"
#include "AbsoluteFault.h"
#include "VarianceFault.h"
#include "ConnectionFault.h"
#include "OffsetFault.h"

Fault::Fault(FederateAttributeReference reference, string& type):
    reference(reference),
    type(type)
{}

list<Fault*> Fault::parse(string& faultScenarioFile) {
    ifstream infile(faultScenarioFile);
    list<Fault*> faults;
    auto faultsInfo = parseKeyValue(infile);
    if (faultsInfo.first != "faults") {
        wcerr << "Invalid start of fault scenario: " << faultsInfo.first.c_str() << endl;
        return faults;
    }
    int nrOfFaults = stoi(faultsInfo.second);
    for (int i = 0; i < nrOfFaults; i++)
        faults.emplace_back(parse(infile));
    infile.close();
    return faults;
}

Fault* Fault::parse(ifstream& infile) {
    string faultType, attributeType;
    getline(infile, faultType, ':');
    auto reference = FederateAttributeReference::parse(infile);
    getline(infile, attributeType, ':');
    if (faultType == FAULT_TYPE_ABSOLUTE)
        return AbsoluteFault::parse(infile, reference, attributeType);
    if (faultType == FAULT_TYPE_OFFSET)
        return OffsetFault::parse(infile, reference, attributeType);
    if (faultType == FAULT_TYPE_CONNECTION)
        return ConnectionFault::parse(infile, reference, attributeType);
    if (faultType == FAULT_TYPE_VARIANCE)
        return VarianceFault::parse(infile, reference, attributeType);
}

FederateAttributeReference Fault::getReference() {
    return reference;
}

string Fault::getType() {
    return type;
}

bool Fault::doApply(long double) {
    return true;
}
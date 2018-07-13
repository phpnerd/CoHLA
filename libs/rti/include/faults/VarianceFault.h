/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

//
// Created by thomas on 14-11-17.
//

#ifndef OPENRTI_LIB_VARIANCEFAULT_H
#define OPENRTI_LIB_VARIANCEFAULT_H


#include <random>
#include <chrono>
#include "Fault.h"

class VarianceFault: public Fault {

public:
    VarianceFault(FederateAttributeReference, string&, double);

    VariableLengthData apply(VariableLengthData) override;

    static Fault* parse(ifstream&, FederateAttributeReference, string&);

private:
    normal_distribution<double>* normalDistribution;
    default_random_engine randomEngine;

};


#endif //OPENRTI_LIB_VARIANCEFAULT_H

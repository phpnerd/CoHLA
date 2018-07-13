/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

//
// Created by thomas on 14-11-17.
//

#include "VarianceFault.h"

VarianceFault::VarianceFault(FederateAttributeReference reference, string& type, double variance):
    Fault::Fault(reference, type)
{
    string seedString = reference.getInstance() + to_string(chrono::system_clock::now().time_since_epoch().count()) + reference.getAttribute();
    seed_seq seed(seedString.begin(), seedString.end());
    normalDistribution = new normal_distribution<double>(0.0, sqrt(variance));
    randomEngine.seed(seed);
}

Fault* VarianceFault::parse(ifstream& infile, FederateAttributeReference reference, string& type) {
    string _variance;
    getline(infile, _variance);
    return new VarianceFault(reference, type, stod(_variance));
}

VariableLengthData VarianceFault::apply(VariableLengthData data) {
    if (type == TYPE_DOUBLE)
        return toVLD<double>(toType<double>(data) + normalDistribution->operator()(randomEngine));
    if (type == TYPE_INT)
        return toVLD<int>((int)((double)toType<int>(data) + normalDistribution->operator()(randomEngine)));
    if (type == TYPE_LONG)
        return toVLD<long>((long)((double)toType<long>(data) + normalDistribution->operator()(randomEngine)));
    return data;
}
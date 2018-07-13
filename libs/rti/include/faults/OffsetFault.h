/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

//
// Created by thomas on 14-11-17.
//

#ifndef OPENRTI_LIB_OFFSETFAULT_H
#define OPENRTI_LIB_OFFSETFAULT_H


#include "TimedFault.h"

class OffsetFault: public TimedFault {

public:
    OffsetFault(FederateAttributeReference, string&, TimeRange, string&);

    VariableLengthData apply(VariableLengthData) override;

    static Fault* parse(ifstream&, FederateAttributeReference, string&);

private:
    string value;

};


#endif //OPENRTI_LIB_OFFSETFAULT_H

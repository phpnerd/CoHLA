/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

//
// Created by thomas on 14-11-17.
//

#ifndef OPENRTI_LIB_CONNECTIONFAULT_H
#define OPENRTI_LIB_CONNECTIONFAULT_H


#include "TimedFault.h"

class ConnectionFault: public TimedFault {

public:
    ConnectionFault(FederateAttributeReference, string&, TimeRange time);

    VariableLengthData apply(VariableLengthData) override;

    static Fault* parse(ifstream&, FederateAttributeReference, string&);

};


#endif //OPENRTI_LIB_CONNECTIONFAULT_H

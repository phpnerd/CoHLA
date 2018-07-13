/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

//
// Created by thomas on 14-11-17.
//

#ifndef OPENRTI_LIB_TIMEDFAULT_H
#define OPENRTI_LIB_TIMEDFAULT_H


#include <models/TimeRange.h>
#include "Fault.h"

class TimedFault: public Fault {

public:
    TimedFault(FederateAttributeReference, string&, TimeRange);

    bool doApply(long double) override;

protected:
    TimeRange time;
    bool applied;

};


#endif //OPENRTI_LIB_TIMEDFAULT_H

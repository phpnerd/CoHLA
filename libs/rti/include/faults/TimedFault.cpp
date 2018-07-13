/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

//
// Created by thomas on 14-11-17.
//

#include "TimedFault.h"

TimedFault::TimedFault(FederateAttributeReference reference, string& type, TimeRange time):
    time(time),
    applied(false),
    Fault::Fault(reference, type)
{}

bool TimedFault::doApply(long double time) {
    return this->time.inRange(time) || (!applied && this->time.before(time));
}
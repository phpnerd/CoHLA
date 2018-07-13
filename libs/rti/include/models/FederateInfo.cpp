/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

//
// Created by thomas on 16-11-17.
//

#include "FederateInfo.h"

FederateInfo::FederateInfo(string& name, string& className, bool watcher):
    name(name),
    className(className),
    watcher(watcher)
{}

string FederateInfo::getName() {
    return name;
}

string FederateInfo::getClass() {
    return className;
}

bool FederateInfo::isWatcher() {
    return watcher;
}
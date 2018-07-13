/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

//
// Created by thomas on 9-2-18.
//

#include "PublishedAttribute.h"

PublishedAttribute::PublishedAttribute(string name):
    name(name),
    changed(false)
{}

string PublishedAttribute::getName() {
    return name;
}

bool PublishedAttribute::hasChanged() {
    bool tmp = changed;
    changed = false;
    return tmp;
}

void PublishedAttribute::update() {
    changed = true;
}
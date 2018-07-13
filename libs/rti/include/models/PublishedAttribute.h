/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

//
// Created by thomas on 9-2-18.
//

#ifndef OPENRTI_LIB_PUBLISHEDATTRIBUTE_H
#define OPENRTI_LIB_PUBLISHEDATTRIBUTE_H

#include <string>

using namespace std;

class PublishedAttribute {

public:
    PublishedAttribute() = default;
    explicit PublishedAttribute(string);

    string getName();
    bool hasChanged();
    void update();

private:
    string name;
    bool changed;

};


#endif //OPENRTI_LIB_PUBLISHEDATTRIBUTE_H

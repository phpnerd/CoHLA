/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

//
// Created by thomas on 9-11-17.
//

#ifndef OPENRTI_LIB_FEDERATEATTRIBUTEREFERENCE_H
#define OPENRTI_LIB_FEDERATEATTRIBUTEREFERENCE_H

#include <cstring>
#include <fstream>
#include "Utils.h"

using namespace std;

class FederateAttributeReference {

public:
    FederateAttributeReference(string, string);
    ~FederateAttributeReference() = default;

    string getInstance();
    void setInstance(string&);
    string getAttribute();
    void setAttribute(string&);

    string getRefName(bool firstUpper = true);

    static FederateAttributeReference parse(ifstream&, bool toEnd = false, char endChar = ':');

    inline bool operator==(FederateAttributeReference r) {
        return instance == r.getInstance() && attribute == r.getAttribute();
    }

private:
    string instance, attribute;

};


#endif //OPENRTI_LIB_FEDERATEATTRIBUTEREFERENCE_H

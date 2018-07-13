/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

//
// Created by thomas on 9-11-17.
//

#ifndef OPENRTI_LIB_CONNECTION_H
#define OPENRTI_LIB_CONNECTION_H

#include <fstream>
#include "FederateAttributeReference.h"

using namespace std;

class Connection {

public:
    Connection(FederateAttributeReference, FederateAttributeReference, string);
    ~Connection() = default;

    FederateAttributeReference getSource();
    void setSource(FederateAttributeReference);
    FederateAttributeReference getTarget();
    void setTarget(FederateAttributeReference);
    string getTypeName();

    static Connection parse(ifstream&);

    inline bool operator==(Connection c) {
        return source == c.getSource() && target == c.getTarget() && typeName == c.getTypeName();
    }

private:
    string typeName;
    FederateAttributeReference source, target;

};


#endif //OPENRTI_LIB_CONNECTION_H

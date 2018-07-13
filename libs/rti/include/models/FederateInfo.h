/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

//
// Created by thomas on 16-11-17.
//

#ifndef OPENRTI_LIB_FEDERATEINFO_H
#define OPENRTI_LIB_FEDERATEINFO_H

#include <string>

using namespace std;

class FederateInfo {

public:
    FederateInfo() = default;
    FederateInfo(string&, string&, bool);

    string getName();
    string getClass();
    bool isWatcher();

private:
    string name, className;
    bool watcher = false;

};


#endif //OPENRTI_LIB_FEDERATEINFO_H

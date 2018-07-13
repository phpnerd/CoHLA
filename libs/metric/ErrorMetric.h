/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

//
// Created by thomas on 3-11-17.
//

#ifndef OPENRTI_LIB_ERRORMETRIC_H
#define OPENRTI_LIB_ERRORMETRIC_H

#include "Metric.h"

template<typename T>
class ErrorMetric : public Metric<T> {

public:
    ErrorMetric(const string&, bool, pair<string, string>, pair<string, string>, string&);

    static Metric<T>* parse(string& name, string&, ifstream& infile);

protected:
    T compute();

private:
    bool squared;

};


#endif //OPENRTI_LIB_ERRORMETRIC_H

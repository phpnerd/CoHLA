/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

//
// Created by thomas on 2-11-17.
//

#ifndef OPENRTI_LIB_ENDVALUEMETRIC_H
#define OPENRTI_LIB_ENDVALUEMETRIC_H

#include "Metric.h"

template<typename T>
class EndValueMetric: public Metric<T> {

public:
    EndValueMetric(const string&, bool, pair<string, string>, string&, bool = false, pair<string, string> = pair<string, string>());

    static EndValueMetric<T>* parse(string, string&, ifstream&);

protected:
    T compute();

private:
    bool absolute;

};

#endif //OPENRTI_LIB_ENDVALUEMETRIC_H
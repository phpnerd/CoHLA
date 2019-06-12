/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

//
// Created by thomas on 20-12-16.
//

#ifndef OPENRTI_LIB_CSVEXPORT_H
#define OPENRTI_LIB_CSVEXPORT_H


#include <federate.h>
#include <limits>
#include <string>
#include <iostream>
#include <fstream>
#include <cmath>

using namespace std;

class CsvExport : public federate {

public:
    explicit CsvExport(string filePath, double measureTime = numeric_limits<double>::max());

    void subscribe() override;
    void stop() override;
    void stopLogging();

    void setConfig(string, string, string, string, string, string) override;

protected:
    map<string, unsigned long> headers;
    map<double, vector<string>> data;
    double measureTime;
    bool done;

    string filePath;

    void updateAttributeValues() override;

    virtual void save();
    void fill();

    void add(long double time, string column, string type, VariableLengthData data);
    void add(long double time, string &column, double value);
    void add(long double time, string &column, float value);
    void add(long double time, string &column, int value);
    void add(long double time, string &column, short value);
    void add(long double time, string &column, long value);
    void add(long double time, string &column, bool value);
    void add(long double time, string &column, string value);

    void setAttribute(string, string) override;
    void setAttribute(string, VariableLengthData) override;
    VariableLengthData getAttribute(string) override;

};


#endif //OPENRTI_LIB_CSVEXPORT_H

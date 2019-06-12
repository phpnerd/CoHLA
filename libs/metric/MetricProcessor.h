/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

//
// Created by thomas on 31-10-17.
//

#ifndef OPENRTI_LIB_METRICPROCESSOR_H
#define OPENRTI_LIB_METRICPROCESSOR_H

#include <federate.h>
#include <limits>
#include <string>
#include <iostream>
#include <fstream>
#include <cmath>
#include "EndValueMetric.h"
#include "ErrorMetric.h"

class MetricProcessor : public federate {

public:
    explicit MetricProcessor(string&, string&, double measureTime = numeric_limits<double>::max());
    ~MetricProcessor() override;
    void init() override;
    void setConfig(string, string, string, string, string);
    void run() override;
    void stop() override;
    void stopLogging();

protected:
    double measureTime;
    bool done;

    void save();

private:
    string metricsFile, exportFile;
    map<string, MetricBase*> metrics;
    list<MetricBase*> metricsList;

    void parseMetrics();

    void timeAdvanceGrantListener(long double) override;
    void updateAttributeValues() override;

    void publishAttributeValues(long double) override;
    void setAttribute(string, string) override;
    void setAttribute(string, VariableLengthData) override;
    VariableLengthData getAttribute(string) override;
    void receiveInteraction(InteractionClassHandle, ParameterHandleValueMap, long double) override;

    bool isFinished();

};


#endif //OPENRTI_LIB_METRICPROCESSOR_H

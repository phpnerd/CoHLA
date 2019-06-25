/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

package nl.ru.sws.cohla.generator.openrti.metric

class MetricFederateMainGenerator {
  
  static def generate() '''
#include <iostream>
#include <cstdlib>
#include <csignal>
#include "MetricCollector.h"

using namespace std;

MetricCollector* metricCollector;

void signal_handler(int signal) {
    if (signal == SIGINT || signal == SIGTERM) {
        metricCollector->stop();
        delete metricCollector;
        exit(0);
    }
}

int main(int argc, char *argv[]) {
    if (signal(SIGINT, signal_handler) == SIG_ERR)
        wcerr << "Failed to register signal_handler for SIGINT" << endl;
    if (signal(SIGTERM, signal_handler) == SIG_ERR)
        wcerr << "Failed to register signal handler for SIGTERM" << endl;
    
    if (argc < 7) {
        cout << "Invalid number of arguments; expected 7: [topology] [metricsFile] [outputFile] [measureTime] [rtiAddress] [scenario | nil] [faultScenario | nil]" << endl;
        return -1;
    }
    string topology = argv[1];
    string metricsFile = argv[2];
    string outputFile = argv[3];
    double measureTime = stod(argv[4]);
    string rtiAddress = argv[5];
    double lookahead = 0.0;
    string scenario = argv[6];
    string faultScenario = argv[7];
    
    metricCollector = new MetricCollector(metricsFile, outputFile, measureTime);
    metricCollector->setConfig("fom.xml", rtiAddress, topology, scenario, faultScenario);
    ((federate*)metricCollector)->init(true, lookahead);
    metricCollector->run();
    metricCollector->stop();
    delete metricCollector;
    return 0;
}

  '''
  
}

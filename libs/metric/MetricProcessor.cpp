/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

//
// Created by thomas on 31-10-17.
//

#include "MetricProcessor.h"

MetricProcessor::MetricProcessor(string &metricsFile, string &exportFile, double measureTime) :
        metricsFile(metricsFile),
        exportFile(exportFile),
        measureTime(measureTime),
        federate::federate()
{
}

MetricProcessor::~MetricProcessor() = default;

void MetricProcessor::init() {
    parseMetrics();
    federate::init();
}

void MetricProcessor::setConfig(string fddFile, string rtiAddress, string topology, string scenario, string faultScenario) {
    federate::setConfig("MetricProcessor", fddFile, rtiAddress, topology, scenario, faultScenario);
}

void MetricProcessor::run() {
    while(!isStopped && measureTime > fed->federateTime && !isFinished())
        advanceNextMessage(measureTime);
}

void MetricProcessor::stop() {
    stopLogging();
}

void MetricProcessor::stopLogging() {
    save();
    federate::stop();
}

void MetricProcessor::timeAdvanceGrantListener(long double time) {
    if (time >= measureTime || isFinished())
        stopLogging();
}

void MetricProcessor::parseMetrics() {
    ifstream infile(metricsFile);
    auto nrOfMetricsInfo = parseKeyValue(infile);
    if (nrOfMetricsInfo.first != "metrics") {
        wcerr << "Failed to parse the number of metrics!" << endl;
        return;
    }
    int nrOfMetrics = stoi(nrOfMetricsInfo.second);
    parseKeyValue(infile); // Ignore measureTime
    for (int i = 0; i < nrOfMetrics; i++) {
        auto metric = MetricBase::parse(infile);
        metrics[metric->getName()] = metric;
        metricsList.push_back(metric);
        Connection c = Connection(FederateAttributeReference(metric->getInstance(), metric->getAttribute()), FederateAttributeReference(federateName, metric->getAttributeName()), metric->getType());
        if (!topology.hasConnection(c))
            topology.addConnection(c);
        if (metric->hasReference()) {
            c = Connection(FederateAttributeReference(metric->getRefInstance(), metric->getRefAttribute()), FederateAttributeReference(federateName, metric->getRefAttributeName()), metric->getType());
            if (!topology.hasConnection(c))
                topology.addConnection(c);
        }
    }
}

void MetricProcessor::updateAttributeValues() {
    for (auto &attribute : subscribedAttributes)
        if (!attribute.isRead())
            for (auto const &metric : metrics)
                metric.second->addData(attribute.getTarget(), attribute.getTime(), attribute.getData());
}

void MetricProcessor::save() {
    if (done)
        return;
    ofstream f;
    f.open (exportFile);
    for (auto const &metric : metrics)
        f << metric.second->getResultLine() << endl;
    f.close();
    done = true;
    wcout << "Results written to \'" << toWString(exportFile) << "\'" << endl;
}

bool MetricProcessor::isFinished() {
    return !exists<MetricBase*>(metricsList, [](MetricBase* metric) {
        return !metric->isFinished();
    });
}

void MetricProcessor::publishAttributeValues(long double) {}
void MetricProcessor::receiveInteraction(InteractionClassHandle icHandle, ParameterHandleValueMap attributes, long double time) {}
void MetricProcessor::setAttribute(string, string) {}
void MetricProcessor::setAttribute(string, VariableLengthData) {}
VariableLengthData MetricProcessor::getAttribute(string) {
    return VariableLengthData();
}
/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

//
// Created by thomas on 20-12-16.
//

#include "CsvExport.h"

CsvExport::CsvExport(string filePath, double measureTime) :
    filePath(filePath),
    measureTime(measureTime),
    done(false),
    federate::federate()
{
}

void CsvExport::setConfig(string federateName, string fddFile, string rtiAddress, string fileTopology, string, string) {
    federate::setConfig(federateName, fddFile, rtiAddress, fileTopology, "nil", "nil");
}

void CsvExport::subscribe() {
    unsigned long i = 0;
    for (auto &subAttr : subscribedAttributes) {
        wcout << "Adding column for " << subAttr.getSourceReference().getRefName(true).c_str() << endl;
        headers[subAttr.getSourceReference().getRefName(true)] = i++;
    }
    federate::subscribe();
}

void CsvExport::stop() {
    stopLogging();
}

void CsvExport::stopLogging() {
    save();
    federate::stop();
}

void CsvExport::save() {
    if (done)
        return;
    fill(); // Provide a value for every cell
    ofstream f;
    f.open (filePath);
    f << "Time";
    vector<string> hds(headers.size());
    for (auto h : headers)
        hds[h.second] = h.first;
    for (string &h : hds)
        f << ";" << h;
    f << "\n";
    for (pair<double, vector<string>> d : data) {
        f << d.first;
        for (string &s : d.second)
            f << ";" << s;
        f << "\n";
    }
    f.close();
    done = true;
    wcout << "Table written to \'" << toWString(filePath) << "\'" << endl;
}

void CsvExport::fill() {
    vector<string> pValues(headers.size());
    for (auto &row_pair : data) {
        for (int i = 0; i < headers.size(); i++) {
            if (!row_pair.second[i].empty())
                pValues[i] = row_pair.second[i];
            else
                row_pair.second[i] = pValues[i];
        }
    }
}

void CsvExport::updateAttributeValues() {
    for (auto &attribute : subscribedAttributes)
        if (!attribute.isRead())
            add(attribute.getTime(), attribute.getSourceReference().getRefName(true), attribute.getType(), attribute.getData());
}

void CsvExport::add(long double time, string column, string type, VariableLengthData data) {
    if (type == TYPE_BOOL)
        return add(time, column, toType<bool>(data));
    else if (type == TYPE_INT)
        return add(time, column, toType<int>(data));
    else if (type == TYPE_LONG)
        return add(time, column, toType<long>(data));
    else if (type == TYPE_DOUBLE)
        return add(time, column, toType<double>(data));
    else if (type == TYPE_STRING)
        return add(time, column, toType<string>(data));
}

void CsvExport::add(long double time, string column, double value) {
    add(time, column, to_string(value));
}

void CsvExport::add(long double time, string column, float value) {
    add(time, column, to_string(value));
}

void CsvExport::add(long double time, string column, int value) {
    add(time, column, to_string(value));
}

void CsvExport::add(long double time, string column, short value) {
    add(time, column, to_string(value));
}

void CsvExport::add(long double time, string column, long value) {
    add(time, column, to_string(value));
}

void CsvExport::add(long double time, string column, bool value) {
    add(time, column, string(value ? "true" : "false"));
}

void CsvExport::add(long double time, string &column, string value) {
    time = round(time * 1.0e5) / 1.0e5;
    if (!done) {
        if (time > measureTime)
            save();
        else {
            if (data.find(time) == data.end())
                data[time] = vector<string>(headers.size());
            if (headers.find(column) == headers.end()) {
                wcerr << "Could not find column " << column.c_str() << endl;
                return;
            }
            data[time].at(headers[column]) = value;
        }
    }
}

void CsvExport::setAttribute(string, VariableLengthData) {}
void CsvExport::setAttribute(string, string) {}
VariableLengthData CsvExport::getAttribute(string) {
    return VariableLengthData();
}
/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

//
// Created by thomas on 1-12-16.
//

#ifndef ORTI_ROOMTHERMOSTAT_FMUWRAPPER_H
#define ORTI_ROOMTHERMOSTAT_FMUWRAPPER_H

#include <iostream>
#include <fmilib.h>
#include <map>
#include <list>
#include <assert.h>
#include <string>
#include <set>

using namespace std;

class FmuWrapper {

public:
    FmuWrapper(string fmuFile, string tmpDir, string name, double tollerance = 1e-6, double startTime = 0.0, double stopTime = 0.0);
    virtual ~FmuWrapper();
    void advanceTime(long double time);
    virtual void updateAttributeValues() = 0;
    double getReal(string name);
    void setReal(string name, double value);
    int getInt(string name);
    void setInt(string name, int value);
    string getString(string name);
    void setString(string name, string value);
    bool getBool(string name);
    void setBool(string name, bool value);
    void finishModelInit();
    double getTime();
    set<string> getChangedAttributes();

protected:
    set<string> changedAttributes;

private:
    fmi2_status_t fmi_status;
    jm_status_enu_t jm_status;
    fmi2_import_t* fmu;
    fmi2_import_variable_list_t* variables;
    jm_callbacks callbacks;
    long double time;
    double tollerance, stopTime;
    map<string, fmi2_value_reference_t> varRefs;

    const string dirPath = "/tmp/fmu";

    void initCallbacks();
    void startModelInit(string fmuPath, string name);
    fmi2_value_reference_t getRef(string name);

};


#endif //ORTI_ROOMTHERMOSTAT_FMUWRAPPER_H

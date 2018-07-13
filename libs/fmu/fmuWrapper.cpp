/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

//
// Created by thomas on 1-12-16.
//

#include "fmuWrapper.h"

FmuWrapper::FmuWrapper(string fmuPath, string tmpDir, string name, double tollerance, double startTime, double stopTime) :
    time(startTime),
    tollerance(tollerance),
    stopTime(stopTime),
    dirPath(tmpDir)
{
    initCallbacks();
    try {
        startModelInit(fmuPath, name);
    } catch (string error) {
        wcerr << "Error: " << error.c_str() << endl;
    }
}

FmuWrapper::~FmuWrapper() {

}

void FmuWrapper::advanceTime(long double time) {
    fmi_status = fmi2_import_do_step(fmu, this->time, time - this->time, true);
    if (fmi_status != fmi2_status_ok)
        throw "Failed to advance to time " + to_string(time);
    this->time = time;
}

void FmuWrapper::initCallbacks() {
    callbacks.malloc = malloc;
    callbacks.calloc = calloc;
    callbacks.realloc = realloc;
    callbacks.free = free;
    callbacks.logger = jm_default_logger;
    callbacks.log_level = jm_log_level_warning;
//    callbacks.log_level = jm_log_level_debug;
    callbacks.context = 0;
}

void FmuWrapper::startModelInit(string fmuPath, string name) {
    fmi_import_context_t* context = fmi_import_allocate_context(&callbacks);
    fmi_version_enu_t version = fmi_import_get_fmi_version(context, fmuPath.c_str(), dirPath.c_str());
    if (version != fmi_version_2_0_enu)
        throw "Wrong FMI version.";
    fmu = fmi2_import_parse_xml(context, dirPath.c_str(), NULL);
    jm_status = fmi2_import_create_dllfmu(fmu, fmi2_fmu_kind_cs, NULL);
    if (jm_status != jm_status_success)
        throw "Failed to import FMU.";
    jm_status = fmi2_import_instantiate(fmu, name.c_str(), fmi2_cosimulation, ("file://" + dirPath).c_str(), true);
    if (jm_status != jm_status_success)
        throw "Failed to instantiate model.";
    fmi_status = fmi2_import_setup_experiment(fmu, tollerance != 1e-6, tollerance, time, stopTime != 0.0, stopTime);
    if (fmi_status != fmi2_status_ok)
        throw "Failed to setup experiment.";
    fmi_status = fmi2_import_enter_initialization_mode(fmu);
    if (fmi_status != fmi2_status_ok)
        throw "Failed to enter initialisation mode.";
    variables = fmi2_import_get_variable_list(fmu, 1);
}

void FmuWrapper::finishModelInit() {
    fmi_status = fmi2_import_exit_initialization_mode(fmu);
    if (fmi_status != fmi2_status_ok)
        throw "Failed to exit model initialisation mode.";
}

double FmuWrapper::getReal(string name) {
    double d[1];
    fmi2_value_reference_t ref[] = { getRef(name) };
    fmi_status = fmi2_import_get_real(fmu, ref, 1, d);
    if (fmi_status != fmi2_status_ok)
        throw "Failed to get real " + name;
    return d[0];
}

void FmuWrapper::setReal(string name, double value) {
    double d[] = { value };
    fmi2_value_reference_t ref[] = { getRef(name) };
    fmi_status = fmi2_import_set_real(fmu, ref, 1, d);
    if (fmi_status != fmi2_status_ok)
        throw "Failed to set real " + name;
}

int FmuWrapper::getInt(string name) {
    int i[1];
    fmi2_value_reference_t ref[] = { getRef(name) };
    fmi_status = fmi2_import_get_integer(fmu, ref, 1, i);
    if (fmi_status != fmi2_status_ok)
        throw "Failed to get int " + name;
    return i[0];
}

void FmuWrapper::setInt(string name, int value) {
    int i[] = { value };
    fmi2_value_reference_t ref[] = { getRef(name) };
    fmi_status = fmi2_import_set_integer(fmu, ref, 1, i);
    if (fmi_status != fmi2_status_ok)
        throw "Failed to set int " + name;
}

string FmuWrapper::getString(string name) {
    fmi2_string_t s[1];
    fmi2_value_reference_t ref[] = { getRef(name) };
    fmi_status = fmi2_import_get_string(fmu, ref, 1, s);
    if (fmi_status != fmi2_status_ok)
        throw "Failed to get string " + name;
    string str((char*)s[0]);
    return str;
}

void FmuWrapper::setString(string name, string value) {
    fmi2_value_reference_t ref[] = { getRef(name) };
    fmi_status = fmi2_import_set_string(fmu, ref, 1, (fmi2_string_t*)value.c_str());
    if (fmi_status != fmi2_status_ok)
        throw "Failed to set string " + name;
}

bool FmuWrapper::getBool(string name) {
    fmi2_boolean_t b[1];
    fmi2_value_reference_t ref[] = { getRef(name) };
    fmi_status = fmi2_import_get_boolean(fmu, ref, 1, b);
    if (fmi_status != fmi2_status_ok)
        throw "Failed to get boolean " + name;
    return (bool)b[0];
}

void FmuWrapper::setBool(string name, bool value) {
    fmi2_boolean_t b[] = { value };
    fmi2_value_reference_t ref[] = { getRef(name) };
    fmi_status = fmi2_import_set_boolean(fmu, ref, 1, b);
    if (fmi_status != fmi2_status_ok)
        throw "Failed to set boolean " + name;
}

double FmuWrapper::getTime() {
    return (double)time;
}

set<string> FmuWrapper::getChangedAttributes() {
    set<string> ca(changedAttributes);
    changedAttributes.clear();
    return ca;
}

fmi2_value_reference_t FmuWrapper::getRef(string name) {
    if (varRefs.count(name) != 1) {
        fmi2_import_variable_t *var;
        for (unsigned int i = 0; i < fmi2_import_get_variable_list_size(variables); i++) {
            var = fmi2_import_get_variable(variables, i);
            if (name == fmi2_import_get_variable_name(fmi2_import_get_variable(variables, i)))
                varRefs[name] = fmi2_import_get_variable_vr(var);
        }
    }
    assert(varRefs[name] > 0);
    return varRefs[name];
}

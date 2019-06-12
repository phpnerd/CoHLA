/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#include "PooslAttributeMessage.h"

using namespace std;

PooslAttributeMessage::PooslAttributeMessage(std::string& process):
    process(process),
    name(""),
    value(""),
    b_value(false),
    PooslMessage(TYPE_ATTR)
{}

PooslAttributeMessage::PooslAttributeMessage(std::string& process, std::string& name):
    process(process),
    name(name),
    value(""),
    b_value(false),
    PooslMessage(TYPE_ATTR)
{}

PooslAttributeMessage::PooslAttributeMessage(std::string& process, std::string& name, std::string& value):
    process(process),
    name(name),
    value(value),
    b_value(true),
    PooslMessage(TYPE_ATTR)
{}

PooslAttributeMessage::PooslAttributeMessage(std::string& process, std::map<std::string, std::string>& values):
    process(process),
    name(""),
    value(""),
    b_value(false),
    values(values),
    PooslMessage(TYPE_ATTR)
{}

PooslAttributeMessage* PooslAttributeMessage::parse(ptree& pt) {
    string process = pt.get<string>(KEYWORD_PROCESS, "");
    string name = pt.get<string>(KEYWORD_NAME, "");
    if (name.empty()) {
        map<string, string> values;
        for (const ptree::value_type& attr : pt.get_child(KEYWORD_VALUES))
            values[attr.first] = attr.second.data();
        return new PooslAttributeMessage(process, values);
    } else {
        string value = pt.get<string>(KEYWORD_VALUE, "");
        return new PooslAttributeMessage(process, name, value);
    }
}

ptree PooslAttributeMessage::getTree() {
    ptree pt;
    pt.put(KEYWORD_TYPE, TYPE_ATTR);
    pt.put(KEYWORD_PROCESS, process);
    if (!name.empty())
        pt.put(KEYWORD_NAME, name);
    if (b_value)
        pt.put(KEYWORD_VALUE, value);
    return pt;
}

PooslAttributeMessage PooslAttributeMessage::makeRequest(std::string& process) {
    return PooslAttributeMessage(process);
}

PooslAttributeMessage PooslAttributeMessage::makeRequest(std::string& process, std::string& name) {
    return PooslAttributeMessage(process, name);
}

std::string PooslAttributeMessage::getProcess() {
    return process;
}

std::string PooslAttributeMessage::getName() {
    return name;
}

std::string PooslAttributeMessage::getValue() {
    return value;
}

void PooslAttributeMessage::setProcess(std::string p) {
    process = move(p);
}

void PooslAttributeMessage::setName(std::string n) {
    name = move(n);
}

void PooslAttributeMessage::setValue(int v) {
    setValue(to_string(v));
}

void PooslAttributeMessage::setValue(double v) {
    setValue(to_string(v));
}

void PooslAttributeMessage::setValue(long v) {
    setValue(to_string(v));
}

void PooslAttributeMessage::setValue(bool v) {
    setValue(to_string(v));
}

void PooslAttributeMessage::setValue(std::string v) {
    b_value = true;
    value = move(v);
}

bool PooslAttributeMessage::hasValue() {
    return b_value;
}

std::map<std::string, std::string> PooslAttributeMessage::getValues() {
    return values;
}

std::string PooslAttributeMessage::getValue(std::string name) {
    if (values.find(name) != values.end())
        return values[name];
    return nullptr;
}

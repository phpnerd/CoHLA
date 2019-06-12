/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#ifndef OPENRTI_LIB_POOSLATTRIBUTEMESSAGE_H
#define OPENRTI_LIB_POOSLATTRIBUTEMESSAGE_H

#define KEYWORD_PROCESS "process"
#define KEYWORD_NAME    "name"
#define KEYWORD_VALUE   "value"
#define KEYWORD_VALUES  "values"

#include <string>
#include <map>

#include "PooslMessage.h"

class PooslAttributeMessage: public PooslMessage {

public:
    explicit PooslAttributeMessage(std::string&);
    PooslAttributeMessage(std::string&, std::string&);
    PooslAttributeMessage(std::string&, std::string&, std::string&);
    PooslAttributeMessage(std::string&, std::map<std::string, std::string>&);

    static PooslAttributeMessage* parse(ptree&);
    ptree getTree() override;
    static PooslAttributeMessage makeRequest(std::string&);
    static PooslAttributeMessage makeRequest(std::string&, std::string&);

    std::string getProcess();
    std::string getName();
    std::string getValue();
    std::map<std::string, std::string> getValues();
    std::string getValue(std::string);

    void setProcess(std::string);
    void setName(std::string);
    void setValue(int);
    void setValue(double);
    void setValue(long);
    void setValue(bool);
    void setValue(std::string);

    bool hasValue();

    bool isAttributeMessage() override {
        return true;
    }

private:
    std::string process, name, value;
    std::map<std::string, std::string> values;
    bool b_value;

};


#endif //OPENRTI_LIB_POOSLATTRIBUTEMESSAGE_H

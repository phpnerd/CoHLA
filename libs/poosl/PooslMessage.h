/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#ifndef OPENRTI_LIB_POOSLMESSAGE_H
#define OPENRTI_LIB_POOSLMESSAGE_H

#define KEYWORD_TYPE    "type"

#define TYPE_TIME   "time"
#define TYPE_ATTR   "attribute"
#define TYPE_INTR   "interaction"

#include <iostream>
#include <string>

#include <boost/property_tree/ptree.hpp>
#include "boost/property_tree/json_parser.hpp"

using boost::property_tree::ptree;

class PooslMessage {

public:
    explicit PooslMessage(std::string);
    ~PooslMessage() = default;

    static PooslMessage* parse(std::string&);
    static PooslMessage* parseMessage(ptree&);
    virtual ptree getTree() = 0;
    std::string serialize();

    std::string getType();

    virtual bool isTimeMessage() {
        return false;
    }

    virtual bool isAttributeMessage() {
        return false;
    }

    virtual bool isInteractionMessage() {
        return false;
    }

protected:
    std::string type;

};


#endif //OPENRTI_LIB_POOSLMESSAGE_H

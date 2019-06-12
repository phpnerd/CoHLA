/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#include "PooslMessage.h"
#include "PooslTimeMessage.h"
#include "PooslAttributeMessage.h"

using namespace std;

PooslMessage::PooslMessage(string type):
    type(std::move(type))
{}

PooslMessage* PooslMessage::parse(string& msg) {
    ptree pt;
    stringstream ss;
    ss << msg;
    read_json(ss, pt);
    return parseMessage(pt);
}

PooslMessage* PooslMessage::parseMessage(ptree& pt) {
    auto type = pt.get<string>(KEYWORD_TYPE);
    if (type == TYPE_TIME)
        return PooslTimeMessage::parse(pt);
    else if (type == TYPE_ATTR)
        return PooslAttributeMessage::parse(pt);
}

string PooslMessage::serialize() {
    stringstream ss;
    write_json(ss, getTree(), false);
    return ss.str();
}

string PooslMessage::getType() {
    return type;
}
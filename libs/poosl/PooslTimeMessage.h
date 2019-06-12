/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#ifndef OPENRTI_LIB_POOSLTIMEMESSAGE_H
#define OPENRTI_LIB_POOSLTIMEMESSAGE_H

#define KEYWORD_STEP    "step"
#define KEYWORD_TIME    "time"

#include "boost/property_tree/json_parser.hpp"

#include "PooslMessage.h"

class PooslTimeMessage: public PooslMessage {

public:
    PooslTimeMessage(long double, long double, bool, bool);

    static PooslTimeMessage* parse(ptree&);
    ptree getTree() override;
    static PooslTimeMessage makeGrant(long double&);
    static PooslTimeMessage makeRequest(long double&);

    long double getStep();
    long double getTime();
    bool isRequest();
    bool isGrant();

    bool isTimeMessage() override {
        return true;
    }

private:
    long double step, time;
    bool b_request = false, b_grant = false;

};


#endif //OPENRTI_LIB_POOSLTIMEMESSAGE_H

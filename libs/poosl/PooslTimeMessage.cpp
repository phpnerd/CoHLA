/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#include "PooslTimeMessage.h"

using namespace std;

PooslTimeMessage::PooslTimeMessage(long double time, long double step, bool grant, bool request):
    time(time),
    step(step),
    b_grant(grant),
    b_request(request),
    PooslMessage(TYPE_TIME)
{}

PooslTimeMessage* PooslTimeMessage::parse(ptree& pt) {
    long double step = pt.get<long double>(KEYWORD_STEP, 0.0);
    long double time = pt.get<long double>(KEYWORD_TIME, 0.0);
    bool grant = time > 0.0, request = step > 0.0;
    return new PooslTimeMessage(time, step, grant, request);
}

ptree PooslTimeMessage::getTree() {
    ptree pt;
    pt.put(KEYWORD_TYPE, TYPE_TIME);
    if (b_request)
        pt.put(KEYWORD_STEP, step);
    if (b_grant) {
        stringstream ss;
        ss << setprecision(9) << time;
        pt.put(KEYWORD_TIME, ss.str());
    }
    return pt;
}

PooslTimeMessage PooslTimeMessage::makeGrant(long double& time) {
    return PooslTimeMessage(time, 0.0, true, false);
}

PooslTimeMessage PooslTimeMessage::makeRequest(long double& step) {
    return PooslTimeMessage(0.0, step, false, true);
}

long double PooslTimeMessage::getStep() {
    return step;
}

long double PooslTimeMessage::getTime() {
    return time;
}

bool PooslTimeMessage::isRequest() {
    return b_request;
}

bool PooslTimeMessage::isGrant() {
    return b_grant;
}

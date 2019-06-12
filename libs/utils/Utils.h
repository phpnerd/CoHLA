/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

//
// Created by thomas on 3-11-17.
//

#ifndef OPENRTI_LIB_UTILS_H
#define OPENRTI_LIB_UTILS_H

#include <cstring>
#include <cstdio>
#include <limits>
#include <list>
#include <map>
#include <functional>
#include <algorithm>
#include <RTI/VariableLengthData.h>
#include <RTI/encoding/BasicDataElements.h>

#define TYPE_DOUBLE     "DOUBLE"
#define TYPE_INT        "INT"
#define TYPE_LONG       "LONG"
#define TYPE_BOOL       "BOOL"
#define TYPE_STRING     "STRING"

#define COMPARATOR_LT   "<"
#define COMPARATOR_LE   "<="
#define COMPARATOR_EQ   "=="
#define COMPARATOR_GE   ">="
#define COMPARATOR_GT   ">"
#define COMPARATOR_NE   "!="

using namespace rti1516e;
using namespace std;

static pair<string, string> parseKeyValue(ifstream &infile) {
    pair<string, string> p;
    getline(infile, p.first, ':');
    getline(infile, p.second);
    return p;
}

static void parseRange(ifstream &infile, pair<double, double> &range) {
    pair<string, string> rangeStr;
    getline(infile, rangeStr.first, ':');
    getline(infile, rangeStr.second, ':');
    range.first = stod(rangeStr.first);
    if (rangeStr.second == "-")
        range.second = numeric_limits<double>::max();
    else
        range.second = stod(rangeStr.second);
}

static void parseInstanceAttribute(ifstream &infile, pair<string, string> &attr, bool toEnd) {
    getline(infile, attr.first, '.');
    if (toEnd)
        getline(infile, attr.second);
    else
        getline(infile, attr.second, ':');
}

template <class T>
static bool exists(list<T>& list, T item) {
    return find(list.begin(), list.end(), item) != list.end();
}

template <class T>
static bool exists(list<T>& list, function<bool(T)> filter) {
    for (T &item : list)
        if (filter(item))
            return true;
    return false;
}

template <class T>
static list<T> filter(list<T>& l, function<bool(T)> filter) {
    list<T> result;
    for (T &item : l)
        if (filter(item))
            result.push_back(item);
    return result;
}

template <class K, class V>
static list<K> keys(map<K, V>& map) {
    list<K> list;
    for (auto const p : map)
        list.push_back(p.first);
    return list;
};

template <class K, class V>
static list<V> values(map<K, V>& map) {
    list<V> list;
    for (auto const p : map)
        list.push_back(p.second);
    return list;
};

template <class K, class V>
static V mapValues(map<K, V>& map, function<V(V, V)> fun) {
    V r = map.begin()->second;
    for (auto const p : map)
        r = fun(r, p.second);
    return r;
};

template<typename T>
static bool compare(T x, T y, string comp) {
    if (comp == COMPARATOR_LT)
        return x < y;
    if (comp == COMPARATOR_LE)
        return x <= y;
    if (comp == COMPARATOR_EQ)
        return x == y;
    if (comp == COMPARATOR_GE)
        return x >= y;
    if (comp == COMPARATOR_GT)
        return x > y;
    if (comp == COMPARATOR_NE)
        return x != y;
    return false;
}

static string toFirstUpper(string str) {
    str[0] = (char) toupper(str[0]);
    return str;
}

template <typename T>
static T toType(const VariableLengthData data) {
    return nullptr;
}

template <>
bool toType<bool>(const VariableLengthData data) {
    HLAboolean b;
    b.decode(data);
    return b.get();
}

template <>
int toType<int>(const VariableLengthData data) {
    if (data.size() == 4) {
        HLAinteger32BE i;
        i.decode(data);
        return i.get();
    } else
        return toType<bool>(data);
}

template <>
long toType<long>(const VariableLengthData data) {
    if (data.size() == 8) {
        HLAinteger64BE i;
        i.decode(data);
        return i.get();
    } else
        return toType<bool>(data);
}

template <>
double toType<double>(const VariableLengthData data) {
    if (data.size() == 8) {
        HLAfloat64BE f;
        f.decode(data);
        return f.get();
    } else
        return toType<bool>(data);
}

template <>
string toType<string>(const VariableLengthData data) {
    HLAASCIIstring s;
    s.decode(data);
    return s.get();
}

template <typename T>
static VariableLengthData toVLD(T data) {
    return VariableLengthData();
}

template<>
VariableLengthData toVLD<bool>(bool data) {
    return HLAboolean(data).encode();
}

template<>
VariableLengthData toVLD<int>(int data) {
    return HLAinteger32BE(data).encode();
}

template<>
VariableLengthData toVLD<long>(long data) {
    return HLAinteger64BE(data).encode();
}

template<>
VariableLengthData toVLD<double>(double data) {
    return HLAfloat64BE(data).encode();
}

template<>
VariableLengthData toVLD<string>(string data) {
    return HLAASCIIstring(data).encode();
}

template <typename T>
static string toString(T data) {
    return "";
}

template <>
string toString<bool>(bool data) {
    return data ? "true" : "false";
}

template <>
string toString<int>(int data) {
    return to_string(data);
}

template <>
string toString<long>(long data) {
    return to_string(data);
}

template <>
string toString<double>(double data) {
    return to_string(data);
}

template <>
string toString<string>(string data) {
    return data;
}

template <typename T>
static T fromString(string s) {}

template <>
bool fromString<bool>(string s) {
    return s == "true";
}

template <>
int fromString<int>(string s) {
    return stoi(s);
}

template <>
long fromString<long>(string s) {
    return stol(s);
}

template <>
double fromString<double>(string s) {
    return stod(s);
}

template <>
string fromString<string>(string s) {
    return s;
}

static VariableLengthData sToVLD(string& s, string& type) {
    if (type == TYPE_BOOL)
        return toVLD<bool>(fromString<bool>(s));
    if (type == TYPE_INT)
        return toVLD<int>(fromString<int>(s));
    if (type == TYPE_LONG)
        return toVLD<long>(fromString<long>(s));
    if (type == TYPE_DOUBLE)
        return toVLD<double>(fromString<double>(s));
    return toVLD<string>(s);
}

#endif //OPENRTI_LIB_UTILS_H

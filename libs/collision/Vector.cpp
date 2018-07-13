/*
 * Copyright (c) Tim Broenink and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

//
// Created by thomas on 8-1-18.
//

#include "Vector.h"

Vector::Vector():
        Vector(0,0,0)
{}

Vector::Vector(vector<float>& values):
    values(values)
{}

Vector::Vector(float x, float y, float z) {
    values = { x, y, z };
}

btVector3 Vector::toVector3() {
    return { values[0], values[1], values[2] };
}

glm::vec3 Vector::toGLMVector3() {
    return { values[0], values[1], values[2] };
}

float Vector::getX() {
    return values[0];
}

float Vector::getY() {
    return values[1];
}

float Vector::getZ() {
    return values[2];
}

float Vector::_get(int index) {
    return values[index];
}

void Vector::setX(float x) {
    values[0] = x;
}

void Vector::setY(float y) {
    values[1] = y;
}

void Vector::setZ(float z) {
    values[2] = z;
}

void Vector::_set(int index, float value) {
    values[index] = value;
}

unsigned long Vector::size() {
    return values.size();
}

string Vector::toString() {
    string s = "(";
    for (int i = 0; i < values.size(); i++)
        s += (i > 0 ? ", " : "") + to_string(values[i]);
    return s + ")";
}

Vector Vector::parse(ptree& tree) {
    vector<float> values;
    for (ptree::value_type& value : tree)
        values.push_back(value.second.get_value<float>());
    return { values };
}
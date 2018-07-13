/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

//
// Created by thomas on 10-11-17.
//

#include "SocketEvent.h"

SocketEvent::SocketEvent(long double time, string& bytes, Socket* socket):
    bytes(bytes),
    socket(socket),
    Event::Event(time)
{}

bool SocketEvent::forFederate(string&) {
    return socket != nullptr;
}

Event* SocketEvent::parse(ifstream& infile, long double time, map<string, Socket*>& sockets) {
    string socketName, bytes;
    getline(infile, socketName, ':');
    getline(infile, bytes);
    return new SocketEvent(time, bytes, sockets[socketName]);
}

void SocketEvent::handle() {
    wcout << "Writing bytes!" << endl;
    socket->writeHexBytes(bytes);
}
/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

//
// Created by thomas on 14-6-17.
//

#include <iostream>
#include <boost/thread.hpp>
#include "socket.h"

using namespace std;

bool stop;

int main(int argc, char *argv[]) {
    if (argc < 3) {
        wcout << "Expected two arguments: [host] [port]" << endl;
        return -1;
    }
    stop = false;
    Socket* sock = new Socket(argv[1], (uint16_t)stoi(argv[2]));
    sock->addCallback([](const unsigned char bytes[]) {
        fprintf(stdout, "Received: %s\n", (const char*)bytes);
        stop = true;
    });
    boost::thread* t = new boost::thread(boost::bind(&Socket::connect, sock, true));
    sock->writeHexBytes("0123456789ABCDEF");
    while(!stop) ;
    wcout << "Now exiting!" << endl;
    delete sock;
}
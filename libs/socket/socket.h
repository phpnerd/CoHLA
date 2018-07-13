/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

//
// Created by thomas on 14-6-17.
//

#ifndef OPENRTI_LIB_SOCKET_H
#define OPENRTI_LIB_SOCKET_H

#include <string>
#include <iostream>
#include <list>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#ifdef LINUX
#include <unistd.h>
#endif
#ifdef WINDOWS
#include <windows.h>
#endif

#define BUFFERSIZE 102400

using namespace std;
using boost::asio::ip::tcp;

class Socket {

public:
    Socket(string, uint16_t);
    virtual ~Socket();
    void connect(bool start_listening);
    void disconnect();
    bool write(const string);
    bool write(vector<unsigned char>);
    bool writeHexBytes(const string);
    size_t read(unsigned char* buffer, size_t length);
    size_t bytesAvailable();

    void addCallback(function<void(unsigned char[])>);

private:
    boost::asio::io_service io_service;
    string host;
    uint16_t port;
    bool connected, stopped;
    shared_ptr<tcp::socket> socket;
    unsigned char buffer[BUFFERSIZE];
    list<function<void(unsigned char[])>> callbacks;

    void listen();
    void waitForConnection();

    vector<unsigned char> strToHex(const string str) {
        vector<unsigned char> bytes;
        for (unsigned int i = 0; i < str.length(); i += 2) {
            std::string byteString = str.substr(i, 2);
            bytes.push_back((unsigned char) strtol(byteString.c_str(), NULL, 16));
        }
        return bytes;
    }

    void sleep(const u_int sleepMs) {
#ifdef LINUX
        usleep(sleepMs * 1000);
#endif
#ifdef WINDOWS
        Sleep(sleepMs);
#endif
    }
};

#endif //OPENRTI_LIB_SOCKET_H

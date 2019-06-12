/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

//
// Created by thomas on 14-6-17.
//

#include "socket.h"

Socket::Socket(string host, uint16_t port):
    host(host),
    port(port),
    connected(false),
    stopped(false)
{}

Socket::~Socket() {
    disconnect();
}

void Socket::connect(bool start_listening) {
    while (!connected) {
        try {
            tcp::endpoint endpoint(boost::asio::ip::address::from_string(host), port);
            socket = make_shared<tcp::socket>(io_service);
            while (true) {
                try {
                    socket->connect(endpoint);
                    break;
                } catch (exception& e) {
                    sleep(250);
                    continue;
                }
            }
            socket->non_blocking(false);
            while (!socket->is_open())
                sleep(250);
            connected = true;
        } catch (exception &e) {
            wcerr << "SocketError: " << e.what() << endl;
            sleep(1000);
        }
    }
    if (start_listening)
        listen();
}

void Socket::listen() {
    try {
        boost::system::error_code error;
        size_t len;
        while (!stopped) {
            len = socket->read_some(boost::asio::buffer(buffer, BUFFERSIZE), error);
            if (error == boost::asio::error::eof)
                break;
            else if (error)
                throw boost::system::system_error(error);
            for (auto callback : callbacks)
                callback(buffer);
        }
    } catch (exception& e) {
        wcerr << "Error: " << e.what() << endl;
    }
}

bool Socket::write(const string str) {
    waitForConnection();
    try {
        socket->write_some(boost::asio::buffer(str.c_str(), strlen(str.c_str())));
        return true;
    } catch (exception& e) {
        wcerr << "Failed to write string: " << e.what() << endl;
        return false;
    }
}

bool Socket::write(vector<unsigned char> bytes) {
    waitForConnection();
    try {
        return socket->write_some(boost::asio::buffer(bytes, bytes.size())) == bytes.size();
    } catch (exception& e) {
        wcerr << "Failed to write bytes: " <<  e.what() << endl;
        return false;
    }
}

size_t Socket::read(unsigned char* buffer, size_t length) {
    try {
        boost::system::error_code error;
        size_t len;
        while (!stopped) {
            len = boost::asio::read(*socket, boost::asio::buffer(this->buffer, length), boost::asio::transfer_exactly(length), error);
            memcpy(buffer, this->buffer, len);

            if (error == boost::asio::error::eof)
                break;
            else if (error)
                throw boost::system::system_error(error);
            return len;
        }
        return 0;
    } catch (exception& e) {
        wcerr << "Error: " << e.what() << endl;
        return 0;
    }
}

string Socket::readLine() {
    try {
        string s;
        boost::system::error_code error;
        boost::asio::streambuf buf;
        boost::asio::read_until(*socket, buf, 0x0a, error);

        if (error)
            throw boost::system::system_error(error);
        istream is(&buf);
        is >> s;
        return s;
    } catch (exception& e) {
        wcerr << "Error: " << e.what() << endl;
        return nullptr;
    }
}

size_t Socket::bytesAvailable() {
    return socket->available();
}

void Socket::waitForConnection() {
    if (!connected || !socket->is_open()) {
        wcout << "Waiting for connection..." << endl;
        while(!connected || !socket->is_open())
            sleep(1000);
    }
}

bool Socket::writeHexBytes(const string hexBytesStr) {
    return write(strToHex(hexBytesStr));
}

void Socket::addCallback(function<void(unsigned char[])> callback) {
    callbacks.push_back(callback);
    wcout << "Added callback. Now containing " << callbacks.size() << " callbacks." << endl;
}

void Socket::disconnect() {
    stopped = true;
    delete &socket;
}
#pragma once

#include <functional>
#include <iostream>
#include <stdio.h>
#include <string>
#include <winsock2.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib") // Winsock Library

class CUdpClient
{
public:
    CUdpClient(const int port)
        : mPort(port)
    {
        WSADATA wsaData;
        int res = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (res != 0)
        {
            std::cout << "WSAStartup failed with error: " << res << std::endl;
            // TODO: do something better
        }
    }

    void start(const std::function<void(char *, int)> &recvCb)
    {
        // Create socket to receive datagrams
        mSockFd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (mSockFd == INVALID_SOCKET)
        {
            std::cout << "Failed open socket. Error " << WSAGetLastError() << std::endl;
            WSACleanup();
            return;
        }

        /* For rebooting the server faster
        int optval = 1;
        setsockopt(mSockFd, SOL_SOCKET, SO_REUSEADDR, (const char *)&optval, sizeof(int));
        */

        // Configure listening on port and interfaces (all)
        struct sockaddr_in recvAddr;
        recvAddr.sin_family = AF_INET;
        recvAddr.sin_port = htons(mPort);
        recvAddr.sin_addr.s_addr = htonl(INADDR_ANY);

        // Bind
        auto res = bind(mSockFd, (struct sockaddr *)&recvAddr, sizeof(recvAddr));
        if (res < 0)
        {
            std::cout << "Server: bind() failed. Error: " << std::to_string(WSAGetLastError()) << std::endl;
            stop();
            return;
        }
        mListen = true;

        const int bufferSize = 4096;
        char buffer[bufferSize];
        struct sockaddr_in clientAddr;
        int clientLen = sizeof(clientAddr);
        int nBytes = 0;
        while (nBytes >= 0 && mListen)
        {
            nBytes = recvfrom(mSockFd, buffer, bufferSize, 0, (struct sockaddr *)&clientAddr, &clientLen);
            if (nBytes < 0 && mListen)
            {
                std::cout << "Error in recvfrom" << std::endl;
                stop();
                return;
            }
            recvCb(buffer, nBytes);
        }
    }

    void stop()
    {
        mListen = false;
        if (mSockFd >= 0)
        {
            closesocket(mSockFd);
        }
        WSACleanup();
    }

private:
    bool mListen{false};
    const int mPort;
    const std::function<void(char *, int)> mReceivedCallback;
    SOCKET mSockFd{INVALID_SOCKET};
};

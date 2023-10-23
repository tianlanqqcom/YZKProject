// Fill out your copyright notice in the Description page of Project Settings.


#include "YGameEntryUserWidget.h"

#include <iostream>
#include <string>

#if PLATFORM_WINDOWS
#include <ws2tcpip.h>
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")
typedef long long ssize_t;
#endif

#if PLATFORM_LINUX || PLATFORM_ANDROID
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif



int32 UYGameEntryUserWidget::CreateRoom()
{
    UE_LOG(LogTemp, Warning, TEXT("Call CreateRoom"));
    #if PLATFORM_WINDOWS
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        // std::cerr << "Failed to initialize Winsock" << std::endl;
        UE_LOG(LogTemp, Warning, L"CreateRoom: Failed to initialize Winsock");
        WSACleanup();
        return 0;
    }

#endif


    // 创建客户端socket
    int clientSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (clientSocket < 0)
    {
        // std::cerr << "无法创建客户端socket！" << std::endl;
        UE_LOG(LogTemp, Warning, L"CreateRoom: Failed to create client socket");
        // WSACleanup();
        return 0;
    }

#if PLATFORM_WINDOWS
    DWORD timeout = 3000;
    if (setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout)) == SOCKET_ERROR)
    {
        std::cerr << "Failed to set socket timeout" << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 0;
    }
#endif

#if PLATFORM_LINUX || PLATFORM_ANDROID
    // 设置超时时间为2秒
    timeval timeout;
    timeout.tv_sec = 3;
    timeout.tv_usec = 0;

    // 设置套接字选项
    if (setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout)) == -1)
    {
        // std::cerr << "无法设置套接字选项" << std::endl;
#if PLATFORM_WINDOWS
        closesocket(clientSocket);
        WSACleanup();
#elif PLATFORM_LINUX || PLATFORM_ANDROID
        close(clientSocket);
#endif
        return 0;
    }
#endif


    // 设置服务器地址和端口
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(7776);
    inet_pton(AF_INET, "62.234.13.91", &(serverAddr.sin_addr));

    // 发送数据给服务器
    std::string requestData = "Create";
    ssize_t bytesSent = sendto(clientSocket, requestData.c_str(), requestData.size(), 0, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    if (bytesSent < 0)
    {
        // std::cerr << "发送数据失败！" << std::endl;
#if PLATFORM_WINDOWS
        closesocket(clientSocket);
        WSACleanup();
#elif PLATFORM_LINUX || PLATFORM_ANDROID
        close(clientSocket);
#endif
        return 0;
    }
    UE_LOG(LogTemp, Warning, TEXT("CreateRoom: Finish send."));
    // 接收服务器返回的数据
    char buffer[1024] = {};
    sockaddr_in serverResponseAddr{};
    socklen_t serverResponseAddrSize = sizeof(serverResponseAddr);

#if PLATFORM_WINDOWS
    int bytesReceived = recvfrom(clientSocket, buffer, sizeof(buffer), 0, (struct sockaddr*)&serverResponseAddr, &serverResponseAddrSize);
    if (bytesReceived == SOCKET_ERROR)
    {
        if (WSAGetLastError() == WSAETIMEDOUT)
        {
            UE_LOG(LogTemp, Warning, TEXT("CreateRoom: Finish send.Timeout"));
            // std::cerr << "Timeout occurred" << std::endl;
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("CreateRoom: Finish send.Other Error"));
            // std::cerr << "Failed to receive data" << std::endl;
        }

#if PLATFORM_WINDOWS
        closesocket(clientSocket);
        WSACleanup();
#elif PLATFORM_LINUX || PLATFORM_ANDROID
        close(clientSocket);
#endif
        return 0;
    }
    
#elif PLATFORM_LINUX || PLATFORM_ANDROID

    ssize_t bytesReceived = recvfrom(clientSocket, buffer, sizeof(buffer), 0, (struct sockaddr*)&serverResponseAddr, &serverResponseAddrSize);
    if (bytesReceived < 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("CreateRoom: Finish recv, but err: %lld."), bytesReceived);

        if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            UE_LOG(LogTemp, Warning, TEXT("CreateRoom: timeout"));
    }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("CreateRoom: Recv failed.errno = %d"), errno);
#if PLATFORM_WINDOWS
            UE_LOG(LogTemp, Warning, TEXT("CreateRoom: Win wsa err = %d"), WSAGetLastError());
#endif

        }
#endif


    

    // 处理接收到的数据
    // std::string responseData(buffer, bytesReceived);
    // std::cout << "接收到的数据: " << responseData << std::endl;

    // 关闭客户端socket
#if PLATFORM_WINDOWS
    closesocket(clientSocket);
    WSACleanup();
#elif PLATFORM_LINUX || PLATFORM_ANDROID
    close(clientSocket);
#endif


    char* EndPtr;
    return strtol(buffer, &EndPtr, 10);
}

TArray<int32> UYGameEntryUserWidget::FindRooms()
{
    UE_LOG(LogTemp, Warning, TEXT("Call FindRooms"));
    TArray<int32> Result;

#if PLATFORM_WINDOWS
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        // std::cerr << "Failed to initialize Winsock" << std::endl;
        UE_LOG(LogTemp, Warning, L"FindRooms: Failed to initialize Winsock");
        WSACleanup();
        return Result;
    }

#endif


    // 创建客户端socket
    int clientSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (clientSocket < 0)
    {
        // std::cerr << "无法创建客户端socket！" << std::endl;
        UE_LOG(LogTemp, Warning, L"FindRooms: Failed to create client socket");
        // WSACleanup();
        return Result;
    }

#if PLATFORM_WINDOWS
    DWORD timeout = 3000;
    if (setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout)) == SOCKET_ERROR)
    {
        std::cerr << "Failed to set socket timeout" << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return Result;
    }
#endif

#if PLATFORM_LINUX || PLATFORM_ANDROID
    // 设置超时时间为2秒
    timeval timeout;
    timeout.tv_sec = 3;
    timeout.tv_usec = 0;

    // 设置套接字选项
    if (setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout)) == -1)
    {
        // std::cerr << "无法设置套接字选项" << std::endl;
#if PLATFORM_WINDOWS
        closesocket(clientSocket);
        WSACleanup();
#elif PLATFORM_LINUX || PLATFORM_ANDROID
        close(clientSocket);
#endif
        return Result;
    }
#endif


    // 设置服务器地址和端口
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(7776);
    inet_pton(AF_INET, "62.234.13.91", &(serverAddr.sin_addr));

    // 发送数据给服务器
    std::string requestData = "Find";
    ssize_t bytesSent = sendto(clientSocket, requestData.c_str(), requestData.size(), 0, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    if (bytesSent < 0)
    {
        // std::cerr << "发送数据失败！" << std::endl;
#if PLATFORM_WINDOWS
        closesocket(clientSocket);
        WSACleanup();
#elif PLATFORM_LINUX || PLATFORM_ANDROID
        close(clientSocket);
#endif
        return Result;
    }
    UE_LOG(LogTemp, Warning, TEXT("FindRooms: Finish send."));
    // 接收服务器返回的数据
    char buffer[1024] = {};
    sockaddr_in serverResponseAddr{};
    socklen_t serverResponseAddrSize = sizeof(serverResponseAddr);

#if PLATFORM_WINDOWS
    int bytesReceived = recvfrom(clientSocket, buffer, sizeof(buffer), 0, (struct sockaddr*)&serverResponseAddr, &serverResponseAddrSize);
    if (bytesReceived == SOCKET_ERROR)
    {
        if (WSAGetLastError() == WSAETIMEDOUT)
        {
            UE_LOG(LogTemp, Warning, TEXT("FindRooms: Finish send.Timeout"));
            // std::cerr << "Timeout occurred" << std::endl;
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("FindRooms: Finish send.Other Error"));
            // std::cerr << "Failed to receive data" << std::endl;
        }

#if PLATFORM_WINDOWS
        closesocket(clientSocket);
        WSACleanup();
#elif PLATFORM_LINUX || PLATFORM_ANDROID
        close(clientSocket);
#endif
        return Result;
    }
    
#elif PLATFORM_LINUX || PLATFORM_ANDROID

    ssize_t bytesReceived = recvfrom(clientSocket, buffer, sizeof(buffer), 0, (struct sockaddr*)&serverResponseAddr, &serverResponseAddrSize);
    if (bytesReceived < 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("FindRooms: Finish recv, but err: %lld."), bytesReceived);

        if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            UE_LOG(LogTemp, Warning, TEXT("FindRooms: timeout"));
    }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("FindRooms: Recv failed.errno = %d"), errno);
#if PLATFORM_WINDOWS
            UE_LOG(LogTemp, Warning, TEXT("FindRooms: Win wsa err = %d"), WSAGetLastError());
#endif

        }
#endif


    

    // 处理接收到的数据
    // std::string responseData(buffer, bytesReceived);
    // std::cout << "接收到的数据: " << responseData << std::endl;

    // 关闭客户端socket
#if PLATFORM_WINDOWS
    closesocket(clientSocket);
    WSACleanup();
#elif PLATFORM_LINUX || PLATFORM_ANDROID
    close(clientSocket);
#endif

    UE_LOG(LogTemp, Warning, TEXT("Call FindRooms: Finish recv, begin process."));
    const int32* ResultArray = reinterpret_cast<int32*>(buffer);

    const int32 ResultLength = ResultArray[0];
    UE_LOG(LogTemp, Warning, TEXT("Call FindRooms: ArrayLength = %d"), ResultLength);

    for(int32 i = 0;i<ResultLength;i++)
    {
        UE_LOG(LogTemp, Warning, TEXT("Call FindRooms: Array[%d] = %d"), i, ResultArray[i + 1]);
        Result.Add(ResultArray[i + 1]);

        if(i == 255)
        {
            break;
        }
    }
    return Result;

}

FString UYGameEntryUserWidget::JoinRoom(int32 TargetPort)
{
    if(TargetPort > 0 && TargetPort <= 65535)
    {
        return FString::Printf(L"62.234.13.91:%d", TargetPort);
    }
    return FString();
}

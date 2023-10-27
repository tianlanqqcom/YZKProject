// Fill out your copyright notice in the Description page of Project Settings.


#include "YGameEntryUserWidget.h"

#include <iostream>
#include <string>
#include <Kismet/GameplayStatics.h>


int UYGameEntryUserWidget::CreateClientSocket()
{
#if PLATFORM_WINDOWS

    // Init WSA
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        WSACleanup();
        return -1;
    }

#endif


    // 创建客户端socket
    int clientSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (clientSocket < 0)
    {
        return -1;
    }

    return clientSocket;
}

void UYGameEntryUserWidget::CloseSocket(int SocketFd)
{
#if PLATFORM_WINDOWS
    closesocket(SocketFd);
    WSACleanup();
#elif PLATFORM_LINUX || PLATFORM_ANDROID
    close(SocketFd);
#endif
}

int UYGameEntryUserWidget::SetSocketTimeout(int SocketFd, int Time_s, int Time_ms)
{
#if PLATFORM_WINDOWS
    DWORD timeout = Time_s * 1000 + Time_ms;
    if (setsockopt(SocketFd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout)) == -1)
    {
        CloseSocket(SocketFd);
        return -1;
    }

#elif PLATFORM_LINUX || PLATFORM_ANDROID
    timeval timeout;
    timeout.tv_sec = Time_s;
    timeout.tv_usec = Time_ms;

    if (setsockopt(SocketFd, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout)) == -1)
    {
        CloseSocket(SocketFd);
        return -1;
    }
#endif
    return 0;
}

int UYGameEntryUserWidget::SendDataToServer(const char* Message, int ClientFd, sockaddr_in* ServerAddr, int ServerAddrLen)
{
    int bytesSent = sendto(ClientFd, Message, strlen(Message), 0, (struct sockaddr*)ServerAddr, ServerAddrLen);
    if (bytesSent < 0)
    {
        CloseSocket(ClientFd);
        return -1;
    }
    return 0;
}

int UYGameEntryUserWidget::RecvFromServer(char* MessageBuffer, int BufferSize, int ClientFd, sockaddr_in* ServerResponseAddr,
    socklen_t* ServerResponseAddrLen)
{
#if PLATFORM_WINDOWS
    int bytesReceived = recvfrom(ClientFd, MessageBuffer, BufferSize, 0, (struct sockaddr*)ServerResponseAddr,
        ServerResponseAddrLen);
    if (bytesReceived == SOCKET_ERROR)
    {
        if (WSAGetLastError() == WSAETIMEDOUT)
        {
            UE_LOG(LogTemp, Warning, L"RecvFromServer:Timeout.");
            CloseSocket(ClientFd);
            return -1; // Timeout
        }

        UE_LOG(LogTemp, Warning, L"RecvFromServer:Other Error = %d", WSAGetLastError());
    	CloseSocket(ClientFd);
    	return -2; // Other Error
    }

#elif PLATFORM_LINUX || PLATFORM_ANDROID

    ssize_t bytesReceived = recvfrom(ClientFd, MessageBuffer, BufferSize, 0, (struct sockaddr*)ServerResponseAddr,
        (socklen_t*)ServerResponseAddrLen);
    if (bytesReceived < 0)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            CloseSocket(ClientFd);
            return -1;
        }
        else
        {
            CloseSocket(ClientFd);
            return -2;
        }
    }
#endif
    return 0;
}

int32 UYGameEntryUserWidget::CreateRoom()
{
    int ClientSocket = CreateClientSocket();
    if (ClientSocket < 0)
    {
        return 0;
    }

    if (SetSocketTimeout(ClientSocket, 3))
    {
        return 0;
    }

    char Message[] = "Create";
    char RecvBuffer[1024] = {};

    sockaddr_in ServerAddr{};
    ServerAddr.sin_family = AF_INET;
    ServerAddr.sin_port = htons(7776);
    inet_pton(AF_INET, "62.234.13.91", &(ServerAddr.sin_addr));

    if (SendDataToServer(Message, ClientSocket, &ServerAddr, sizeof(ServerAddr)))
    {
        UE_LOG(LogTemp, Warning, TEXT("CreateRoom: Send Failed."));
        return 0;
    }

    sockaddr_in ServerResponseAddr{};
    socklen_t ServerResponseAddrSize = sizeof(ServerResponseAddr);

    if (RecvFromServer(RecvBuffer, sizeof(RecvBuffer), ClientSocket, &ServerResponseAddr, &ServerResponseAddrSize))
    {
        UE_LOG(LogTemp, Warning, TEXT("CreateRoom: Recv Failed."));
        return 0;
    }

    CloseSocket(ClientSocket);

    char* EndPtr;
    return strtol(RecvBuffer, &EndPtr, 10);
}

TArray<int32> UYGameEntryUserWidget::FindRooms()
{
	UE_LOG(LogTemp, Warning, TEXT("Call FindRooms"));
	TArray<int32> Result;

	int ClientSocket = CreateClientSocket();
	if (ClientSocket < 0)
	{
		return Result;
	}

	if (SetSocketTimeout(ClientSocket, 3))
	{
		return Result;
	}

	char Message[] = "Find";
	char RecvBuffer[1024] = {};

	sockaddr_in ServerAddr{};
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(7776);
	inet_pton(AF_INET, "62.234.13.91", &(ServerAddr.sin_addr));

	if (SendDataToServer(Message, ClientSocket, &ServerAddr, sizeof(ServerAddr)))
	{
		UE_LOG(LogTemp, Warning, TEXT("FindRooms: Send Failed."));
		return Result;
	}

	sockaddr_in ServerResponseAddr{};
	socklen_t ServerResponseAddrSize = sizeof(ServerResponseAddr);

	if (RecvFromServer(RecvBuffer, sizeof(RecvBuffer), ClientSocket, &ServerResponseAddr, &ServerResponseAddrSize))
	{
		UE_LOG(LogTemp, Warning, TEXT("FindRooms: Recv Failed."));
		return Result;
	}
	CloseSocket(ClientSocket);

	UE_LOG(LogTemp, Warning, TEXT("Call FindRooms: Finish recv, begin process."));
	const int32* ResultArray = reinterpret_cast<int32*>(RecvBuffer);

	const int32 ResultLength = ResultArray[0];
	UE_LOG(LogTemp, Warning, TEXT("Call FindRooms: ArrayLength = %d"), ResultLength);

	for (int32 i = 0; i < ResultLength; i++)
	{
		UE_LOG(LogTemp, Warning, TEXT("Call FindRooms: Array[%d] = %d"), i, ResultArray[i + 1]);
		Result.Add(ResultArray[i + 1]);

		if (i == 255)
		{
			break;
		}
	}
	return Result;

}

FString UYGameEntryUserWidget::JoinRoom(int32 TargetPort)
{
    if (TargetPort > 0 && TargetPort <= 65535)
    {
        return FString::Printf(TEXT("62.234.13.91:%d"), TargetPort);
    }
    return FString();
}

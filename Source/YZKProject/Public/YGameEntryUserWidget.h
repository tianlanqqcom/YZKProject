// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

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

#include "YGameEntryUserWidget.generated.h"



UCLASS()
class YZKPROJECT_API UYGameEntryUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	int32 CreateRoom();

	UFUNCTION(BlueprintCallable)
	TArray<int32> FindRooms(bool& bIsNetSuccess);

	UFUNCTION(BlueprintCallable)
	FString JoinRoom(int32 TargetPort);

private:
	// Net Functions
	static int CreateClientSocket();

	static void CloseSocket(int SocketFd);

	static int SetSocketTimeout(int SocketFd, int Time_s, int Time_ms = 0);

	static int SendDataToServer(const char* Message, int ClientFd, struct sockaddr_in* ServerAddr, int ServerAddrLen);

	static int RecvFromServer(char* MessageBuffer, int BufferSize, int ClientFd, sockaddr_in* ServerResponseAddr, socklen_t* ServerResponseAddrLen);
};

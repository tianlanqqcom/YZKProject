// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "YGameEntryUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class YZKPROJECT_API UYGameEntryUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	int32 CreateRoom();

	UFUNCTION(BlueprintCallable)
	TArray<int32> FindRooms();

	UFUNCTION(BlueprintCallable)
	FString JoinRoom(int32 TargetPort);
	
};

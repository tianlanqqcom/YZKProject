// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "YPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerNameChanged);
/**
 * 
 */
UCLASS()
class YZKPROJECT_API AYPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintCallable)
	FOnPlayerNameChanged OnPlayerNameChanged;

	virtual void OnRep_PlayerName() override;

	AYPlayerState();

	UFUNCTION(BlueprintPure)
	FORCEINLINE FString GetYPlayerName() const
	{
		return GetPlayerNameCustom();
	}
	
};

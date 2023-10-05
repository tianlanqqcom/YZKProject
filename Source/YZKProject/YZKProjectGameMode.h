// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "YZKProjectGameMode.generated.h"

UCLASS(minimalapi)
class AYZKProjectGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AYZKProjectGameMode();

	virtual void StartPlay() override;
};




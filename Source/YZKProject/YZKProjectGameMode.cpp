// Copyright Epic Games, Inc. All Rights Reserved.

#include "YZKProjectGameMode.h"
#include "YZKProjectCharacter.h"
#include "UObject/ConstructorHelpers.h"

AYZKProjectGameMode::AYZKProjectGameMode()
{
	// set default pawn class to our Blueprinted character
	//static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/BP_YCharacter"));
	//if (PlayerPawnBPClass.Class != NULL)
	//{
	//	DefaultPawnClass = PlayerPawnBPClass.Class;
	//}
}

void AYZKProjectGameMode::StartPlay()
{
	Super::StartPlay();


}

// Fill out your copyright notice in the Description page of Project Settings.


#include "YNFPSGameModeBase.h"

#include <string>

FString ToString(int32 X, int32 Y)
{
	std::string s = "";
	s += std::to_string(X);
	s += " ";
	s += std::to_string(Y);

	return FString(s.c_str());
}

void AYNFPSGameModeBase::StartPlay()
{
	Super::StartPlay();

#if WITH_EDITOR
	check(GEngine != nullptr);
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, L"Using YN FPS Game Mode Base.");

	int32 ViewX, ViewY;
	GetWorld()->GetFirstPlayerController()->GetViewportSize(ViewX, ViewY);

	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, ToString(ViewX, ViewY));
#endif
	
}



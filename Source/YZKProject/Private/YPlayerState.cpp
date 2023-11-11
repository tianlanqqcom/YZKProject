// Fill out your copyright notice in the Description page of Project Settings.


#include "YPlayerState.h"

void AYPlayerState::OnRep_PlayerName()
{
	Super::OnRep_PlayerName();
}

AYPlayerState::AYPlayerState()
{
	if(!OnPlayerNameChanged.IsBound())
	{
		OnPlayerNameChanged.AddDynamic(this, &AYPlayerState::OnRep_PlayerName);
	}
}

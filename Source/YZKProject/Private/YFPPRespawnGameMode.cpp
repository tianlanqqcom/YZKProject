// Fill out your copyright notice in the Description page of Project Settings.


#include "YFPPRespawnGameMode.h"
#include "YNFPPCharacter.h"
#include "YNFPSGameModeBase.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/GameStateBase.h"

void AYFPPRespawnGameMode::BeginPlay()
{
    Super::BeginPlay();

#if WITH_EDITOR
    check(GEngine)
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, "Using AYFPPRespawnGameMode.");
#endif

	this->MaxPlayerCount = 3;

    //将我们的玩家已死亡委托绑定到GameMode的PlayerDied函数。
    if (!OnPlayerDied.IsBound())
    {
        OnPlayerDied.AddDynamic(this, &AYFPPRespawnGameMode::PlayerDied);
    }

	UE_LOG(LogTemp, Warning, TEXT("Close Handel Registered."));
	GetWorld()->GetTimerManager().SetTimer(CloseServerHandle, this, &AYFPPRespawnGameMode::CloseServerAndReturnToMain, 15.0f, true);

}

void AYFPPRespawnGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	// 获取当前关卡中的玩家数量
	int32 RemainingPlayers = GetWorld()->GetGameState()->PlayerArray.Num();

	if(RemainingPlayers >= MaxPlayerCount)
	{
		ErrorMessage = FString::Printf(TEXT("Too Much Players, Now Already has: %d"), RemainingPlayers);
	}
	else
	{
		Super::PreLogin(Options, Address, UniqueId, ErrorMessage);
	}

}

APlayerController* AYFPPRespawnGameMode::Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	ClientOptionString = Options;
	return Super::Login(NewPlayer, InRemoteRole, Portal, Options, UniqueId, ErrorMessage);
}

void AYFPPRespawnGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
}

void AYFPPRespawnGameMode::CloseServerAndReturnToMain()
{
	if(GetWorld()->GetGameState()->PlayerArray.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("Server is ready to close, Reason: No player exists."));
		FPlatformMisc::RequestExit(true);
	}
	// GetWorld()->GetTimerManager().ClearTimer(CloseServerHandle);
}

void AYFPPRespawnGameMode::RestartPlayer(AController* NewPlayer)
{
    Super::RestartPlayer(NewPlayer);
}

void AYFPPRespawnGameMode::RestartPlayerByYZK(AController* NewPlayer)
{
	// if player is or will be destroyed, return
	if (NewPlayer == nullptr || NewPlayer->IsPendingKillPending())
	{
		return;
	}

	// Try to get restart spot.
	AActor* StartSpot = FindPlayerStart(NewPlayer);

	// If a start spot wasn't found,
	if (StartSpot == nullptr)
	{
		// Check for a previously assigned spot
		if (NewPlayer->StartSpot != nullptr)
		{
			StartSpot = NewPlayer->StartSpot.Get();
			UE_LOG(LogGameMode, Warning, TEXT("RestartPlayer: Player start not found, using last start spot"));
		}
	}

	if (!StartSpot)
	{
		UE_LOG(LogGameMode, Warning, TEXT("RestartPlayerByYZK at GetPlayerStartSpot: Player start not found"));
		return;
	}

	FRotator SpawnRotation = StartSpot->GetActorRotation();
	FVector SpawnPosition = StartSpot->GetActorLocation();

	// Get player pawn.
	if (NewPlayer->GetPawn() != nullptr)
	{
		// If we have an existing pawn, just use it's rotation
		// SpawnRotation = NewPlayer->GetPawn()->GetActorRotation();
		NewPlayer->GetPawn()->SetActorLocation(SpawnPosition);
		NewPlayer->GetPawn()->SetActorRotation(SpawnRotation);
	}
	else if (GetDefaultPawnClassForController(NewPlayer) != nullptr)
	{
		// Try to create a pawn to use of the default class for this player
		APawn* NewPawn = SpawnDefaultPawnFor(NewPlayer, StartSpot);
		if (IsValid(NewPawn))
		{
			NewPlayer->SetPawn(NewPawn);
			NewPlayer->GetPawn()->SetActorLocation(SpawnPosition);
			NewPlayer->GetPawn()->SetActorRotation(SpawnRotation);
		}
		else
		{
			UE_LOG(LogGameMode, Warning, TEXT("RestartPlayerByYZK: Creating Character, but is not valid."));
			return;
		}
	}
	else
	{
		UE_LOG(LogGameMode, Warning, TEXT("RestartPlayerByYZK: Unable to create Character"));
		return;
	}

	// Set Character rotation and transform.
	// Set initial control rotation to starting rotation rotation
	// NewPlayer->ClientSetRotation(NewPlayer->GetPawn()->GetActorRotation(), true);
	// NewPlayer->ClientSetLocation(SpawnPosition, SpawnRotation);

	FRotator NewControllerRot = SpawnRotation;
	NewControllerRot.Roll = 0.f;
	NewPlayer->SetControlRotation(NewControllerRot);

	// SetPlayerDefaults(NewPlayer->GetPawn());
}

void AYFPPRespawnGameMode::PlayerDied(ACharacter* Character)
{
    //获得角色玩家控制器的引用
    AController* CharacterController = Character->GetController();
    RestartPlayer(CharacterController);
}

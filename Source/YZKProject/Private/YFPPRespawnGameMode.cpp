// Fill out your copyright notice in the Description page of Project Settings.


#include "YFPPRespawnGameMode.h"
#include "YNFPPCharacter.h"
#include "UObject/ConstructorHelpers.h"

void AYFPPRespawnGameMode::BeginPlay()
{
    Super::BeginPlay();

#if WITH_EDITOR
    check(GEngine)
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, "Using AYFPPRespawnGameMode.");
#endif

    //将我们的玩家已死亡委托绑定到GameMode的PlayerDied函数。
    if (!OnPlayerDied.IsBound())
    {
        OnPlayerDied.AddDynamic(this, &AYFPPRespawnGameMode::PlayerDied);
    }

}

void AYFPPRespawnGameMode::RestartPlayer(AController* NewPlayer)
{
    Super::RestartPlayer(NewPlayer);
}

void AYFPPRespawnGameMode::PlayerDied(ACharacter* Character)
{
    //获得角色玩家控制器的引用
    AController* CharacterController = Character->GetController();
    RestartPlayer(CharacterController);
}

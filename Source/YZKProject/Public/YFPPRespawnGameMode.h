// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "YFPPRespawnGameMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerDiedSignature, ACharacter*, Character);
/**
 * 
 */
UCLASS()
class YZKPROJECT_API AYFPPRespawnGameMode : public AGameMode
{
	GENERATED_BODY()

public:
    FORCEINLINE const FOnPlayerDiedSignature& GetOnPlayerDied() const { return OnPlayerDied; }

    // 尝试生成玩家的Pawn。
    virtual void RestartPlayer(AController* NewPlayer) override;

    // 暂时弃用 - 新的重生方法
    void RestartPlayerByYZK(AController* NewPlayer);

protected:
    virtual void BeginPlay() override;

    //在玩家角色死亡时调用。
    UFUNCTION()
    virtual void PlayerDied(ACharacter* Character);

    //要绑定委托的签名。 
    UPROPERTY()
    FOnPlayerDiedSignature OnPlayerDied;
};

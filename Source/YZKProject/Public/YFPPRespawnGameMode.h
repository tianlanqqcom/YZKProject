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

    UPROPERTY(BlueprintReadOnly)
    FString ClientOptionString;

    // 尝试生成玩家的Pawn。
    virtual void RestartPlayer(AController* NewPlayer) override;

    // 暂时弃用 - 新的重生方法
    void RestartPlayerByYZK(AController* NewPlayer);

    virtual APlayerController* Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage);

protected:
    int32 MaxPlayerCount;

    virtual void BeginPlay() override;

    virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;

    virtual void Logout(AController* Exiting) override;

    void CloseServerAndReturnToMain();

    FTimerHandle CloseServerHandle;

    //在玩家角色死亡时调用。
    UFUNCTION()
    virtual void PlayerDied(ACharacter* Character);

    //要绑定委托的签名。 
    UPROPERTY()
    FOnPlayerDiedSignature OnPlayerDied;
};

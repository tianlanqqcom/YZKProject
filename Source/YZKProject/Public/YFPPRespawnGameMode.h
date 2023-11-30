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

	/**
	 * \brief 客户端传递的路径选项字符串，在本例中常为 .../MyLevel?YName=XXX
	 * 在Login的时候保存一个副本，方便后续初始化玩家姓名
	 */
	UPROPERTY(BlueprintReadOnly)
    FString ClientOptionString;

    // 尝试生成玩家的Pawn。
    virtual void RestartPlayer(AController* NewPlayer) override;

    /** 新的重生方法，基本同RsestartPlayer */
    void RestartPlayerByYZK(AController* NewPlayer);

    virtual APlayerController* Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage);

protected:
    int32 MaxPlayerCount;

	/**
	 * 当玩家数大于MaxPlayerCount时，拒绝后续玩家加入
	 */
	virtual void BeginPlay() override;

    virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;

    virtual void Logout(AController* Exiting) override;

    void CloseServerAndReturnToMain();

	/**
	 * 关闭服务器的延时句柄，15s没有玩家即关闭
	 */
	FTimerHandle CloseServerHandle;

    //在玩家角色死亡时调用。
    UFUNCTION()
    virtual void PlayerDied(ACharacter* Character);

    //要绑定委托的签名。 
    UPROPERTY()
    FOnPlayerDiedSignature OnPlayerDied;
};

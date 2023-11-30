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
	 * \brief �ͻ��˴��ݵ�·��ѡ���ַ������ڱ����г�Ϊ .../MyLevel?YName=XXX
	 * ��Login��ʱ�򱣴�һ�����������������ʼ���������
	 */
	UPROPERTY(BlueprintReadOnly)
    FString ClientOptionString;

    // ����������ҵ�Pawn��
    virtual void RestartPlayer(AController* NewPlayer) override;

    /** �µ���������������ͬRsestartPlayer */
    void RestartPlayerByYZK(AController* NewPlayer);

    virtual APlayerController* Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage);

protected:
    int32 MaxPlayerCount;

	/**
	 * �����������MaxPlayerCountʱ���ܾ�������Ҽ���
	 */
	virtual void BeginPlay() override;

    virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;

    virtual void Logout(AController* Exiting) override;

    void CloseServerAndReturnToMain();

	/**
	 * �رշ���������ʱ�����15sû����Ҽ��ر�
	 */
	FTimerHandle CloseServerHandle;

    //����ҽ�ɫ����ʱ���á�
    UFUNCTION()
    virtual void PlayerDied(ACharacter* Character);

    //Ҫ��ί�е�ǩ���� 
    UPROPERTY()
    FOnPlayerDiedSignature OnPlayerDied;
};

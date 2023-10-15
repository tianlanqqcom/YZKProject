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

    // ����������ҵ�Pawn��
    virtual void RestartPlayer(AController* NewPlayer) override;

    // ��ʱ���� - �µ���������
    void RestartPlayerByYZK(AController* NewPlayer);

protected:
    virtual void BeginPlay() override;

    //����ҽ�ɫ����ʱ���á�
    UFUNCTION()
    virtual void PlayerDied(ACharacter* Character);

    //Ҫ��ί�е�ǩ���� 
    UPROPERTY()
    FOnPlayerDiedSignature OnPlayerDied;
};

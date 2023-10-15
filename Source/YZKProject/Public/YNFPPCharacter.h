// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/GameViewportClient.h"
#include "YProjectile.h"
#include "YNFPPCharacter.generated.h"


UCLASS()
class YZKPROJECT_API AYNFPPCharacter : public ACharacter
{
	GENERATED_BODY()
private:
    FRotator RecordLastCameraRotator;

    // 角色是否正在开火中
    bool bIsFiring = false;

    // 距离上次开火的间隔时间
    float TimeAgainstLastFire = 0.0f;

    double RecordLastPitch = 0.0;

    bool bCharacterHasDied = false;

public:
    UPROPERTY(ReplicatedUsing = OnRep_Score)
    int32 Score;

    UFUNCTION()
    void OnScoreUpdate();

    UFUNCTION()
    FORCEINLINE void OnRep_Score()
    {
        OnScoreUpdate();
    }

    UPROPERTY(ReplicatedUsing = OnRep_LastHitPerson)
    AYNFPPCharacter* LastHitPerson;

    UFUNCTION()
    void OnLastHitPersonUpdate();

    UFUNCTION()
    FORCEINLINE void OnRep_LastHitPerson()
    {
        OnLastHitPersonUpdate();
    }

    // 为此角色的属性设置默认值
    AYNFPPCharacter();

    /** 属性复制 */
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
    // 开火最小间隔，蓝图可设置
    UPROPERTY(EditAnywhere)
    float MinTimeDistanceOfFire = 0.5f;

    UPROPERTY(EditDefaultsOnly, Category = "Health")
    float MaxHealthPoint = 1.0f;

    UPROPERTY(ReplicatedUsing = OnRep_CurrentHealth)
    float HealthPoint = 1.0f;

    /** RepNotify，用于同步对当前生命值所做的更改。*/
    UFUNCTION()
    void OnRep_CurrentHealth();
     
    /** 响应要更新的生命值。修改后，立即在服务器上调用，并在客户端上调用以响应RepNotify*/
    void OnHealthUpdate();

    UPROPERTY(EditAnywhere)
    float WeaponPitchMuzzle = 5.0f;

public:
    /** 最大生命值的取值函数。*/
    UFUNCTION(BlueprintPure, Category = "Health")
    FORCEINLINE float GetMaxHealth() const { return MaxHealthPoint; }

    /** 当前生命值的取值函数。*/
    UFUNCTION(BlueprintPure, Category = "Health")
    FORCEINLINE float GetCurrentHealth() const { return HealthPoint; }

    UFUNCTION(BlueprintPure, Category = "GamePlay")
    FORCEINLINE int32 GetCurrentScore() const { return Score; }

    UFUNCTION(BlueprintPure, Category = "GamePlay")
    FORCEINLINE AYNFPPCharacter* GetCurrentLastHitPerson() const { return LastHitPerson; }

    /** 当前生命值的存值函数。将此值的范围限定在0到MaxHealth之间，并调用OnHealthUpdate。仅在服务器上调用。*/
    UFUNCTION(BlueprintCallable, Category = "Health")
    void SetCurrentHealth(float healthValue);

    UFUNCTION(BlueprintCallable, Category = "GamePlay")
    void SetCurrentScore(int32 NewScore);

    UFUNCTION(BlueprintCallable, Category = "GamePlay")
    void SetLastHitPerson(AYNFPPCharacter* NewLastHitPerson);

    /** 承受伤害的事件。从APawn覆盖。*/
    UFUNCTION(BlueprintCallable, Category = "Health")
    virtual float TakeDamage(float DamageTaken, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

protected:
    FTimerHandle RespawnDelayHandle;

    // 当游戏开始或重生（Spawn）时被调用
    virtual void BeginPlay() override;

    //当我们的Actor在游戏过程中被销毁时调用。
    virtual void Destroyed() override;

    //调用GameMode类以重新启动玩家角色。
    void CallRestartPlayer();

    // 要生成的发射物类。
    UPROPERTY(EditAnywhere, Category = Projectile)
    TSubclassOf<class AYProjectile> ProjectileClass;

public:
    UFUNCTION(BlueprintCallable)
    virtual void RespawnTimeDelay();

    // 每一帧都被调用
    virtual void Tick(float DeltaTime) override;

    // 被调用，将功能与输入绑定
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    // 处理用于前后移动的输入。
    UFUNCTION()
    void MoveForward(float Value);

    // 处理用于左右移动的输入。
    UFUNCTION()
    void MoveRight(float Value);

    // 按下键时，设置跳跃标记。
    UFUNCTION()
    void StartJump();

    // 释放键时，清除跳跃标记。
    UFUNCTION()
    void StopJump();

    // 发射发射物的函数。
    UFUNCTION(Server, Reliable)
    void Fire();

    // 当开火按钮按下时调用
    UFUNCTION(BlueprintCallable)
    void StartFire();

    // 当开火按钮持续按下时调用，尝试开火，如果不满足最小时间间隔则忽略本次尝试
    UFUNCTION(BlueprintCallable)
    void TryFire();

    // 当开火按钮松开时调用
    UFUNCTION(BlueprintCallable)
    void StopFire();

    // FPS摄像机
    UPROPERTY(BlueprintReadWrite)
    UCameraComponent* FPSCameraComponent;

    // 第一人称网格体（手臂），仅对所属玩家可见。
    UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
    USkeletalMeshComponent* FPSMesh;

    // 枪口相对于摄像机位置的偏移。
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
    FVector MuzzleOffset;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input")
    bool bIsRotating;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    float Sensitivity = 0.2f;

    FVector PreviousLocation;

    void OnTouchPressed(ETouchIndex::Type FingerIndex, FVector Location);

    void OnTouchReleased(ETouchIndex::Type FingerIndex, FVector Location);

    void OnTouchMoved(ETouchIndex::Type FingerIndex, FVector Location);

};

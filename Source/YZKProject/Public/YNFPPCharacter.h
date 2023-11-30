// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/GameViewportClient.h"
#include "YProjectile.h"

#include  "PlayerTouchProcesser.h"
#include "YNFPPCharacter.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerStateChanged);

UCLASS()
class YZKPROJECT_API AYNFPPCharacter : public ACharacter
{
	GENERATED_BODY()
private:
    /**
     * 为了隔离触摸输入而添加，主要是为了解决右半屏输入冲突。
     * 输入处理器仅在正常玩家（有图形化界面的）的角色上初始化，
     * 如果玩家有图形界面，将在BeginPlay时初始化。
     */
    FPlayerTouchProcesser TouchProcesser;

	/**
	 * 记录上一帧的镜头旋转值。
	 */
	FRotator RecordLastCameraRotator;

    /**
     * 角色是否正在开火中。
     */
    bool bIsFiring = false;

    /**
     * 距离上次开火的间隔时间。当该间隔时间大于枪械的开火间隔时才能开火。
     */
    float TimeAgainstLastFire = 0.0f;

    /**
	 * 记录上一帧的偏转角。
	 */
    double RecordLastPitch = 0.0;

    /**
	 * 记录角色是否死亡。
	 */
    bool bCharacterHasDied = false;

    /**
     * 已废弃
     * 记录是否启用UMG输入
     */
    UPROPERTY(Replicated)
    bool bEnableUMGInput = true;

protected:

    /** 开火最小间隔 */
    UPROPERTY(EditAnywhere)
    float MinTimeDistanceOfFire = 0.5f;

    /** 玩家血量上限为1，有效的血量在0-1 */
    UPROPERTY(EditDefaultsOnly, Category = "Health")
    float MaxHealthPoint = 1.0f;

    /** 玩家当前生命 */
    UPROPERTY(ReplicatedUsing = OnRep_CurrentHealth)
    float HealthPoint = 1.0f;

    /** 武器的偏移角度 */
    UPROPERTY(EditAnywhere)
    float WeaponPitchMuzzle = 5.0f;

    /** 重生延时句柄 */
    FTimerHandle RespawnDelayHandle;

    /** 隐藏死亡界面延时 */
    FTimerHandle HideDieMenuDelayHandle;

    /** 重新启用输入延时 */
    FTimerHandle ServerReEnableInputHandle;

    /** 重新启用输入延时 */
    FTimerHandle ClientReEnableInputHandle;

    /** 要生成的发射物类。*/
    UPROPERTY(EditAnywhere, Category = Projectile)
    TSubclassOf<class AYProjectile> ProjectileClass;

public:
    /**
     * 公开玩家分数，当击杀敌人时更新
     */
    UPROPERTY(ReplicatedUsing = OnRep_Score)
    int32 Score;

    /**
     * 标记上次命中的敌人
     */
    UPROPERTY(ReplicatedUsing = OnRep_LastHitPerson)
    AYNFPPCharacter* LastHitPerson;

    /**
     * 无用的
     * 玩家的名字
     *
     * 注：该值实际上保存在PlayerState
     */
    UPROPERTY(ReplicatedUsing = OnRep_Name)
    FString Name;

    /**
     * 创建委托实例
     */
    UPROPERTY(BlueprintCallable)
    FOnPlayerStateChanged OnPlayerStateChanged;

    /** FPS摄像机 */
    UPROPERTY(BlueprintReadWrite)
    UCameraComponent* FPSCameraComponent;

    /** 第一人称网格体（手臂），仅对所属玩家可见。 */
    UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
    USkeletalMeshComponent* FPSMesh;

    /** 枪口相对于摄像机位置的偏移。 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
    FVector MuzzleOffset;

    /** 角色是否正在旋转 */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input")
    bool bIsRotating;

    /** 灵敏度 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    float Sensitivity = 0.2f;

    /** 上一次记录的位置 */
    FVector PreviousLocation;

public:
    /**
	 * 构造函数
	 */
    AYNFPPCharacter();

    /** 属性复制 */
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    /**
     * 当玩家死亡时，显示死亡界面。
     */
    UFUNCTION(BlueprintImplementableEvent)
	void ShowDieMenu();


    /**
     * 当玩家复活时，隐藏死亡界面。
     */
    UFUNCTION(BlueprintImplementableEvent)
    void HideDieMenu();

    /**
	 * 禁用输入，等价于DisableInput
	 */
    UFUNCTION(BlueprintImplementableEvent)
    void CallDisableInput();

    /**
     * 启用输入，等价于EnableInput
     *
     * 关于为什么会有这两个函数：最开始使用Disable/Enable的时候出现了Bug，然后尝试使用蓝图解决，依然有Bug,
     * 最后虽然解决了Bug，但这里已经成了历史遗留问题，不便修改
     */
    UFUNCTION(BlueprintImplementableEvent)
    void CallEnableInput();

    /**
     * 获取bEnableUMGInput
     */
    UFUNCTION(BlueprintPure)
    FORCEINLINE bool IsEnableUMGInput() const
    {
        return bEnableUMGInput;
    }

    /** 更新玩家分数 */
    UFUNCTION()
    void OnScoreUpdate();

    UFUNCTION()
    FORCEINLINE void OnRep_Score()
    {
        OnScoreUpdate();
    }

    /** 更新上次命中的人 */
    UFUNCTION()
    void OnLastHitPersonUpdate();

    UFUNCTION()
    FORCEINLINE void OnRep_LastHitPerson()
    {
        OnLastHitPersonUpdate();
    }

    /** 获取玩家名 */
    UFUNCTION(BlueprintPure)
    FORCEINLINE FString GetCharacterName() const
    {
        return Name;
    }

    /** 设置玩家名 */
    UFUNCTION(BlueprintCallable)
    FORCEINLINE void SetCharacterName(const FString& NewName)
    {
        this->Name = NewName;
        OnNameUpdate();
    }

    /** 当玩家名更新时调用 */
    UFUNCTION()
    void OnNameUpdate();

    UFUNCTION()
    FORCEINLINE void OnRep_Name()
    {
        OnNameUpdate();
    }

    /** 当玩家状态更改时调用 */
    virtual void OnRep_PlayerState() override;

    /** 最大生命值的取值函数。*/
    UFUNCTION(BlueprintPure, Category = "Health")
    FORCEINLINE float GetMaxHealth() const { return MaxHealthPoint; }

    /** 当前生命值的取值函数。*/
    UFUNCTION(BlueprintPure, Category = "Health")
    FORCEINLINE float GetCurrentHealth() const { return HealthPoint; }

    /** 获取当前分数 */
    UFUNCTION(BlueprintPure, Category = "GamePlay")
    FORCEINLINE int32 GetCurrentScore() const { return Score; }

    /** 获取上一个击中的敌人 */
    UFUNCTION(BlueprintPure, Category = "GamePlay")
    FORCEINLINE AYNFPPCharacter* GetCurrentLastHitPerson() const { return LastHitPerson; }

    /** 当前生命值的存值函数。将此值的范围限定在0到MaxHealth之间，并调用OnHealthUpdate。仅在服务器上调用。*/
    UFUNCTION(BlueprintCallable, Category = "Health")
    void SetCurrentHealth(float healthValue);

    /** 设置当前分数 */
    UFUNCTION(BlueprintCallable, Category = "GamePlay")
    void SetCurrentScore(int32 NewScore);

    /** 设置上次击中的敌人 */
    UFUNCTION(BlueprintCallable, Category = "GamePlay")
    void SetLastHitPerson(AYNFPPCharacter* NewLastHitPerson);

    /** 承受伤害的事件。从APawn覆盖。*/
    UFUNCTION(BlueprintCallable, Category = "Health")
    virtual float TakeDamage(float DamageTaken, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

    /** 重生的延迟函数 */
    UFUNCTION(BlueprintCallable)
    virtual void RespawnTimeDelay();

    /** 每一帧都被调用 */
    virtual void Tick(float DeltaTime) override;

    /** 被调用，将功能与输入绑定 */
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

    /** 当手指按下时 */
    void OnTouchPressed(ETouchIndex::Type FingerIndex, FVector Location);

    /** 当手指按下时 */
    void OnTouchReleased(ETouchIndex::Type FingerIndex, FVector Location);

    /** 当手指按下时 */
    void OnTouchMoved(ETouchIndex::Type FingerIndex, FVector Location);

protected:

    /** RepNotify，用于同步对当前生命值所做的更改。*/
    UFUNCTION()
    void OnRep_CurrentHealth();

    /** 响应要更新的生命值。修改后，立即在服务器上调用，并在客户端上调用以响应RepNotify */
    void OnHealthUpdate();

    // 当游戏开始或重生（Spawn）时被调用
    virtual void BeginPlay() override;

    //当我们的Actor在游戏过程中被销毁时调用。
    virtual void Destroyed() override;

    //调用GameMode类以重新启动玩家角色。
    void CallRestartPlayer();
};

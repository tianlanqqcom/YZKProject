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
    FPlayerTouchProcesser TouchProcesser;

    FRotator RecordLastCameraRotator;

    // ��ɫ�Ƿ����ڿ�����
    bool bIsFiring = false;

    // �����ϴο���ļ��ʱ��
    float TimeAgainstLastFire = 0.0f;

    double RecordLastPitch = 0.0;

    bool bCharacterHasDied = false;

    UPROPERTY(Replicated)
    bool bEnableUMGInput = true;

public:
    UFUNCTION(BlueprintImplementableEvent)
	void ShowDieMenu();

    UFUNCTION(BlueprintImplementableEvent)
    void HideDieMenu();

    UFUNCTION(BlueprintImplementableEvent)
    void CallDisableInput();

    UFUNCTION(BlueprintImplementableEvent)
    void CallEnableInput();

    UFUNCTION(BlueprintPure)
    FORCEINLINE bool IsEnableUMGInput() const
    {
        return bEnableUMGInput;
    }

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

    UPROPERTY(ReplicatedUsing = OnRep_Name)
    FString Name;

    UFUNCTION(BlueprintPure)
    FORCEINLINE FString GetCharacterName() const
    {
        return Name;
    }

    UFUNCTION(BlueprintCallable)
    FORCEINLINE void SetCharacterName(const FString& NewName)
    {
        this->Name = NewName;
        OnNameUpdate();
    }

    UFUNCTION()
    void OnNameUpdate();

    UFUNCTION()
    FORCEINLINE void OnRep_Name()
    {
        OnNameUpdate();
    }

    // Ϊ�˽�ɫ����������Ĭ��ֵ
    AYNFPPCharacter();

    /** ���Ը��� */
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UPROPERTY(BlueprintCallable)
    FOnPlayerStateChanged OnPlayerStateChanged;
    virtual void OnRep_PlayerState() override;

protected:
    // ������С�������ͼ������
    UPROPERTY(EditAnywhere)
    float MinTimeDistanceOfFire = 0.5f;

    UPROPERTY(EditDefaultsOnly, Category = "Health")
    float MaxHealthPoint = 1.0f;

    UPROPERTY(ReplicatedUsing = OnRep_CurrentHealth)
    float HealthPoint = 1.0f;

    /** RepNotify������ͬ���Ե�ǰ����ֵ�����ĸ��ġ�*/
    UFUNCTION()
    void OnRep_CurrentHealth();
     
    /** ��ӦҪ���µ�����ֵ���޸ĺ������ڷ������ϵ��ã����ڿͻ����ϵ�������ӦRepNotify*/
    void OnHealthUpdate();

    UPROPERTY(EditAnywhere)
    float WeaponPitchMuzzle = 5.0f;

public:
    /** �������ֵ��ȡֵ������*/
    UFUNCTION(BlueprintPure, Category = "Health")
    FORCEINLINE float GetMaxHealth() const { return MaxHealthPoint; }

    /** ��ǰ����ֵ��ȡֵ������*/
    UFUNCTION(BlueprintPure, Category = "Health")
    FORCEINLINE float GetCurrentHealth() const { return HealthPoint; }

    UFUNCTION(BlueprintPure, Category = "GamePlay")
    FORCEINLINE int32 GetCurrentScore() const { return Score; }

    UFUNCTION(BlueprintPure, Category = "GamePlay")
    FORCEINLINE AYNFPPCharacter* GetCurrentLastHitPerson() const { return LastHitPerson; }

    /** ��ǰ����ֵ�Ĵ�ֵ����������ֵ�ķ�Χ�޶���0��MaxHealth֮�䣬������OnHealthUpdate�����ڷ������ϵ��á�*/
    UFUNCTION(BlueprintCallable, Category = "Health")
    void SetCurrentHealth(float healthValue);

    UFUNCTION(BlueprintCallable, Category = "GamePlay")
    void SetCurrentScore(int32 NewScore);

    UFUNCTION(BlueprintCallable, Category = "GamePlay")
    void SetLastHitPerson(AYNFPPCharacter* NewLastHitPerson);

    /** �����˺����¼�����APawn���ǡ�*/
    UFUNCTION(BlueprintCallable, Category = "Health")
    virtual float TakeDamage(float DamageTaken, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

protected:
    FTimerHandle RespawnDelayHandle;

    FTimerHandle HideDieMenuDelayHandle;

    FTimerHandle ServerReEnableInputHandle;

    FTimerHandle ClientReEnableInputHandle;

    // ����Ϸ��ʼ��������Spawn��ʱ������
    virtual void BeginPlay() override;

    //�����ǵ�Actor����Ϸ�����б�����ʱ���á�
    virtual void Destroyed() override;

    //����GameMode��������������ҽ�ɫ��
    void CallRestartPlayer();

    // Ҫ���ɵķ������ࡣ
    UPROPERTY(EditAnywhere, Category = Projectile)
    TSubclassOf<class AYProjectile> ProjectileClass;

public:
    UFUNCTION(BlueprintCallable)
    virtual void RespawnTimeDelay();

    // ÿһ֡��������
    virtual void Tick(float DeltaTime) override;

    // �����ã��������������
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    // ��������ǰ���ƶ������롣
    UFUNCTION()
    void MoveForward(float Value);

    // �������������ƶ������롣
    UFUNCTION()
    void MoveRight(float Value);

    // ���¼�ʱ��������Ծ��ǡ�
    UFUNCTION()
    void StartJump();

    // �ͷż�ʱ�������Ծ��ǡ�
    UFUNCTION()
    void StopJump();

    // ���䷢����ĺ�����
    UFUNCTION(Server, Reliable)
    void Fire();

    // ������ť����ʱ����
    UFUNCTION(BlueprintCallable)
    void StartFire();

    // ������ť��������ʱ���ã����Կ��������������Сʱ��������Ա��γ���
    UFUNCTION(BlueprintCallable)
    void TryFire();

    // ������ť�ɿ�ʱ����
    UFUNCTION(BlueprintCallable)
    void StopFire();

    // FPS�����
    UPROPERTY(BlueprintReadWrite)
    UCameraComponent* FPSCameraComponent;

    // ��һ�˳������壨�ֱۣ�������������ҿɼ���
    UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
    USkeletalMeshComponent* FPSMesh;

    // ǹ������������λ�õ�ƫ�ơ�
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

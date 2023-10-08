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
    // ��ɫ�Ƿ����ڿ�����
    bool bIsFiring = false;

    // �����ϴο���ļ��ʱ��
    float TimeAgainstLastFire = 0.0f;

    double RecordLastPitch = 0.0;

public:
    // Ϊ�˽�ɫ����������Ĭ��ֵ
    AYNFPPCharacter();

    /** ���Ը��� */
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

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

    /** ��ǰ����ֵ�Ĵ�ֵ����������ֵ�ķ�Χ�޶���0��MaxHealth֮�䣬������OnHealthUpdate�����ڷ������ϵ��á�*/
    UFUNCTION(BlueprintCallable, Category = "Health")
    void SetCurrentHealth(float healthValue);

    /** �����˺����¼�����APawn���ǡ�*/
    UFUNCTION(BlueprintCallable, Category = "Health")
    virtual float TakeDamage(float DamageTaken, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

protected:
    // ����Ϸ��ʼ��������Spawn��ʱ������
    virtual void BeginPlay() override;

    // Ҫ���ɵķ������ࡣ
    UPROPERTY(EditAnywhere, Category = Projectile)
    TSubclassOf<class AYProjectile> ProjectileClass;

public:
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

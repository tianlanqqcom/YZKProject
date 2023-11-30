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
     * Ϊ�˸��봥���������ӣ���Ҫ��Ϊ�˽���Ұ��������ͻ��
     * ���봦��������������ң���ͼ�λ�����ģ��Ľ�ɫ�ϳ�ʼ����
     * ��������ͼ�ν��棬����BeginPlayʱ��ʼ����
     */
    FPlayerTouchProcesser TouchProcesser;

	/**
	 * ��¼��һ֡�ľ�ͷ��תֵ��
	 */
	FRotator RecordLastCameraRotator;

    /**
     * ��ɫ�Ƿ����ڿ����С�
     */
    bool bIsFiring = false;

    /**
     * �����ϴο���ļ��ʱ�䡣���ü��ʱ�����ǹе�Ŀ�����ʱ���ܿ���
     */
    float TimeAgainstLastFire = 0.0f;

    /**
	 * ��¼��һ֡��ƫת�ǡ�
	 */
    double RecordLastPitch = 0.0;

    /**
	 * ��¼��ɫ�Ƿ�������
	 */
    bool bCharacterHasDied = false;

    /**
     * �ѷ���
     * ��¼�Ƿ�����UMG����
     */
    UPROPERTY(Replicated)
    bool bEnableUMGInput = true;

protected:

    /** ������С��� */
    UPROPERTY(EditAnywhere)
    float MinTimeDistanceOfFire = 0.5f;

    /** ���Ѫ������Ϊ1����Ч��Ѫ����0-1 */
    UPROPERTY(EditDefaultsOnly, Category = "Health")
    float MaxHealthPoint = 1.0f;

    /** ��ҵ�ǰ���� */
    UPROPERTY(ReplicatedUsing = OnRep_CurrentHealth)
    float HealthPoint = 1.0f;

    /** ������ƫ�ƽǶ� */
    UPROPERTY(EditAnywhere)
    float WeaponPitchMuzzle = 5.0f;

    /** ������ʱ��� */
    FTimerHandle RespawnDelayHandle;

    /** ��������������ʱ */
    FTimerHandle HideDieMenuDelayHandle;

    /** ��������������ʱ */
    FTimerHandle ServerReEnableInputHandle;

    /** ��������������ʱ */
    FTimerHandle ClientReEnableInputHandle;

    /** Ҫ���ɵķ������ࡣ*/
    UPROPERTY(EditAnywhere, Category = Projectile)
    TSubclassOf<class AYProjectile> ProjectileClass;

public:
    /**
     * ������ҷ���������ɱ����ʱ����
     */
    UPROPERTY(ReplicatedUsing = OnRep_Score)
    int32 Score;

    /**
     * ����ϴ����еĵ���
     */
    UPROPERTY(ReplicatedUsing = OnRep_LastHitPerson)
    AYNFPPCharacter* LastHitPerson;

    /**
     * ���õ�
     * ��ҵ�����
     *
     * ע����ֵʵ���ϱ�����PlayerState
     */
    UPROPERTY(ReplicatedUsing = OnRep_Name)
    FString Name;

    /**
     * ����ί��ʵ��
     */
    UPROPERTY(BlueprintCallable)
    FOnPlayerStateChanged OnPlayerStateChanged;

    /** FPS����� */
    UPROPERTY(BlueprintReadWrite)
    UCameraComponent* FPSCameraComponent;

    /** ��һ�˳������壨�ֱۣ�������������ҿɼ��� */
    UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
    USkeletalMeshComponent* FPSMesh;

    /** ǹ������������λ�õ�ƫ�ơ� */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
    FVector MuzzleOffset;

    /** ��ɫ�Ƿ�������ת */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input")
    bool bIsRotating;

    /** ������ */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    float Sensitivity = 0.2f;

    /** ��һ�μ�¼��λ�� */
    FVector PreviousLocation;

public:
    /**
	 * ���캯��
	 */
    AYNFPPCharacter();

    /** ���Ը��� */
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    /**
     * ���������ʱ����ʾ�������档
     */
    UFUNCTION(BlueprintImplementableEvent)
	void ShowDieMenu();


    /**
     * ����Ҹ���ʱ�������������档
     */
    UFUNCTION(BlueprintImplementableEvent)
    void HideDieMenu();

    /**
	 * �������룬�ȼ���DisableInput
	 */
    UFUNCTION(BlueprintImplementableEvent)
    void CallDisableInput();

    /**
     * �������룬�ȼ���EnableInput
     *
     * ����Ϊʲô�����������������ʼʹ��Disable/Enable��ʱ�������Bug��Ȼ����ʹ����ͼ�������Ȼ��Bug,
     * �����Ȼ�����Bug���������Ѿ�������ʷ�������⣬�����޸�
     */
    UFUNCTION(BlueprintImplementableEvent)
    void CallEnableInput();

    /**
     * ��ȡbEnableUMGInput
     */
    UFUNCTION(BlueprintPure)
    FORCEINLINE bool IsEnableUMGInput() const
    {
        return bEnableUMGInput;
    }

    /** ������ҷ��� */
    UFUNCTION()
    void OnScoreUpdate();

    UFUNCTION()
    FORCEINLINE void OnRep_Score()
    {
        OnScoreUpdate();
    }

    /** �����ϴ����е��� */
    UFUNCTION()
    void OnLastHitPersonUpdate();

    UFUNCTION()
    FORCEINLINE void OnRep_LastHitPerson()
    {
        OnLastHitPersonUpdate();
    }

    /** ��ȡ����� */
    UFUNCTION(BlueprintPure)
    FORCEINLINE FString GetCharacterName() const
    {
        return Name;
    }

    /** ��������� */
    UFUNCTION(BlueprintCallable)
    FORCEINLINE void SetCharacterName(const FString& NewName)
    {
        this->Name = NewName;
        OnNameUpdate();
    }

    /** �����������ʱ���� */
    UFUNCTION()
    void OnNameUpdate();

    UFUNCTION()
    FORCEINLINE void OnRep_Name()
    {
        OnNameUpdate();
    }

    /** �����״̬����ʱ���� */
    virtual void OnRep_PlayerState() override;

    /** �������ֵ��ȡֵ������*/
    UFUNCTION(BlueprintPure, Category = "Health")
    FORCEINLINE float GetMaxHealth() const { return MaxHealthPoint; }

    /** ��ǰ����ֵ��ȡֵ������*/
    UFUNCTION(BlueprintPure, Category = "Health")
    FORCEINLINE float GetCurrentHealth() const { return HealthPoint; }

    /** ��ȡ��ǰ���� */
    UFUNCTION(BlueprintPure, Category = "GamePlay")
    FORCEINLINE int32 GetCurrentScore() const { return Score; }

    /** ��ȡ��һ�����еĵ��� */
    UFUNCTION(BlueprintPure, Category = "GamePlay")
    FORCEINLINE AYNFPPCharacter* GetCurrentLastHitPerson() const { return LastHitPerson; }

    /** ��ǰ����ֵ�Ĵ�ֵ����������ֵ�ķ�Χ�޶���0��MaxHealth֮�䣬������OnHealthUpdate�����ڷ������ϵ��á�*/
    UFUNCTION(BlueprintCallable, Category = "Health")
    void SetCurrentHealth(float healthValue);

    /** ���õ�ǰ���� */
    UFUNCTION(BlueprintCallable, Category = "GamePlay")
    void SetCurrentScore(int32 NewScore);

    /** �����ϴλ��еĵ��� */
    UFUNCTION(BlueprintCallable, Category = "GamePlay")
    void SetLastHitPerson(AYNFPPCharacter* NewLastHitPerson);

    /** �����˺����¼�����APawn���ǡ�*/
    UFUNCTION(BlueprintCallable, Category = "Health")
    virtual float TakeDamage(float DamageTaken, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

    /** �������ӳٺ��� */
    UFUNCTION(BlueprintCallable)
    virtual void RespawnTimeDelay();

    /** ÿһ֡�������� */
    virtual void Tick(float DeltaTime) override;

    /** �����ã�������������� */
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

    /** ����ָ����ʱ */
    void OnTouchPressed(ETouchIndex::Type FingerIndex, FVector Location);

    /** ����ָ����ʱ */
    void OnTouchReleased(ETouchIndex::Type FingerIndex, FVector Location);

    /** ����ָ����ʱ */
    void OnTouchMoved(ETouchIndex::Type FingerIndex, FVector Location);

protected:

    /** RepNotify������ͬ���Ե�ǰ����ֵ�����ĸ��ġ�*/
    UFUNCTION()
    void OnRep_CurrentHealth();

    /** ��ӦҪ���µ�����ֵ���޸ĺ������ڷ������ϵ��ã����ڿͻ����ϵ�������ӦRepNotify */
    void OnHealthUpdate();

    // ����Ϸ��ʼ��������Spawn��ʱ������
    virtual void BeginPlay() override;

    //�����ǵ�Actor����Ϸ�����б�����ʱ���á�
    virtual void Destroyed() override;

    //����GameMode��������������ҽ�ɫ��
    void CallRestartPlayer();
};

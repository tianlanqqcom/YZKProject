// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <GameFramework/SpringArmComponent.h>

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "YCharacter.generated.h"

UCLASS()
class YZKPROJECT_API AYCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	/*UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	class AActor* EmptyObj;*/

	UPROPERTY(EditAnywhere)
	float ScaleSizeOfForwardVector = 20.0f;

	UPROPERTY(EditAnywhere)
	FVector DispOfProjectile;

public:
	// Sets default values for this character's properties
	AYCharacter();

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Ҫ���ɵķ������ࡣ
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class AYProjectile> ProjectileClass;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

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

	// ������������ĺ�����
	UFUNCTION()
	void Fire();

	// ǹ������������λ�õ�ƫ�ơ�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	FVector MuzzleOffset;

	float DistanceToCamera = 250.0f;

};

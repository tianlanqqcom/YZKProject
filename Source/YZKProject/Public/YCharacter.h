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

	// 要生成的发射物类。
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

	// 处理发射物射击的函数。
	UFUNCTION()
	void Fire();

	// 枪口相对于摄像机位置的偏移。
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	FVector MuzzleOffset;

	float DistanceToCamera = 250.0f;

};

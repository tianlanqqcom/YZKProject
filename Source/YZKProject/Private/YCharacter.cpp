// Fill out your copyright notice in the Description page of Project Settings.

#include "YCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "YProjectile.h"

// Sets default values
AYCharacter::AYCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

// Called when the game starts or when spawned
void AYCharacter::BeginPlay()
{
	Super::BeginPlay();
#if WITH_EDITOR
	check(GEngine != nullptr);
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("We are using YCharacter."));
#endif

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	
}

// Called every frame
void AYCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AYCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{

		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AYCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AYCharacter::Look);

	}

	 //// 设置"移动"绑定。
	 //PlayerInputComponent->BindAxis("MoveForward", this, &AYCharacter::MoveForward);
	 //PlayerInputComponent->BindAxis("MoveRight", this, &AYCharacter::MoveRight);

	 //// 设置"观看"绑定。
	 //PlayerInputComponent->BindAxis("Turn", this, &AYCharacter::AddControllerYawInput);
	 //PlayerInputComponent->BindAxis("LookUp", this, &AYCharacter::AddControllerPitchInput);

	 //// 设置"操作"绑定。
	 //PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AYCharacter::StartJump);
	 //PlayerInputComponent->BindAction("Jump", IE_Released, this, &AYCharacter::StopJump);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AYCharacter::Fire);

}

void AYCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AYCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AYCharacter::MoveForward(float Value)
{
	// 找出"前进"方向，并记录玩家想向该方向移动。
	FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::X);
	AddMovementInput(Direction, Value);
}

void AYCharacter::MoveRight(float Value)
{
	// 找出"右侧"方向，并记录玩家想向该方向移动。
	FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::Y);
	AddMovementInput(Direction, Value);
}

void AYCharacter::StartJump()
{
	bPressedJump = true;
}

void AYCharacter::StopJump()
{
	bPressedJump = false;
}

void AYCharacter::Fire()
{
	if(!ProjectileClass)
	{
#if WITH_EDITOR
		// Debug Messages
		check(GEngine != nullptr);
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, 
			TEXT("Attemp Fire but ProjectileClass is nullptr"));
#endif
		return;

	}
	// 试图发射发射物。
	if (ProjectileClass)
	{
		// 获取角色在世界中的位置
		FVector CharacterLocation = GetActorLocation();

		// 获取角色的向前方向向量
		FVector CharacterForwardVector = GetActorForwardVector();

		
		// 获取摄像机变换。
		FVector CameraLocation;
		FRotator CameraRotation = GetViewRotation();

		FVector CameraBoomLocation = GetPawnViewLocation();

		FVector CameraForwardVector = CameraRotation.Vector();

		if(CharacterForwardVector.Dot(CameraForwardVector) < 0)
		{
#if WITH_EDITOR
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red,
				TEXT("Attemp Fire Towards Behind."));
#endif
			return;
		}

		// GetActorEyesViewPoint(CameraLocation, CameraRotation);

		// 设置MuzzleOffset，在略靠近摄像机前生成发射物。
		// MuzzleOffset.Set(100.0f, 0.0f, 0.0f);

		// 将MuzzleOffset从摄像机空间变换到世界空间。
		// FVector MuzzleLocation = CameraLocation + FTransform(CameraRotation).TransformVector(MuzzleOffset);
		// FVector MuzzleLocation = CameraBoomLocation;

		// FVector MuzzleLocation = EmptyObj->GetActorLocation();

		// 使目标方向略向上倾斜。
		FVector MuzzleLocation(CharacterLocation.X + DispOfProjectile.X,
			CharacterLocation.Y + DispOfProjectile.Y,
			CharacterLocation.Z + DispOfProjectile.Z);

		FRotator MuzzleRotation = CameraRotation;

		UWorld* World = GetWorld();
		if(!World)
		{
#if WITH_EDITOR
			check(GEngine != nullptr);
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red,
				TEXT("Attemp Fire but Actor is not spawned in a level"));
#endif
			return;
		}

		if (World)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = GetInstigator();

			// 在枪口位置生成发射物。
			AYProjectile* Projectile = World->SpawnActor<AYProjectile>(ProjectileClass, MuzzleLocation, MuzzleRotation, SpawnParams);
			if (Projectile)
			{
				// 设置发射物的初始轨迹。
				FVector LaunchDirection = MuzzleRotation.Vector();

#if WITH_EDITOR
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red,
					(LaunchDirection.ToString()));
				// GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red,
				// 	TEXT("Create projectil"));
#endif

				Projectile->FireInDirection(LaunchDirection);
			}
			else
			{
#if WITH_EDITOR
				check(GEngine != nullptr);
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red,
					TEXT("Failed to create projectile"));
#endif
			}
		}
	}
}

//void AYCharacter::Fire()
//{
//	if (!ProjectileClass)
//	{
//#if WITH_EDITOR
//		// Debug Messages
//		check(GEngine != nullptr);
//		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Attempt Fire but ProjectileClass is nullptr"));
//#endif
//		return;
//	}
//
//	// 获取摄像机变换。
//	FVector CameraLocation;
//	FRotator CameraRotation;
//	GetActorEyesViewPoint(CameraLocation, CameraRotation);
//
//	FVector SpringArmLocation = CameraBoom->GetComponentLocation();
//	FRotator SpringArmRotation = CameraBoom->GetComponentRotation();
//
//	// FVector MuzzleLocation = SpringArmLocation + FTransform(CameraRotation).TransformVector(MuzzleOffset);
//	// 计算枪口位置。
//	FVector MuzzleLocation = SpringArmLocation + SpringArmRotation.Vector() * 1.0f;
//	// 计算枪口位置。
//	// FVector MuzzleLocation = CameraLocation + (GetActorLocation() - CameraLocation).GetSafeNormal() * CameraBoom->G;
//
//	// 获取弹簧臂组件的位置。
//	// FVector MuzzleLocation = CameraBoom->GetComponentLocation();
//	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, MuzzleLocation.ToString());
//
//	// 使目标方向略向上倾斜。
//	FRotator MuzzleRotation = CameraRotation;
//	MuzzleRotation.Pitch += 1.0f;
//
//	UWorld* World = GetWorld();
//	if (!World)
//	{
//#if WITH_EDITOR
//		check(GEngine != nullptr);
//		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Attempt Fire but Actor is not spawned in a level"));
//#endif
//		return;
//	}
//
//	if (World)
//	{
//		FActorSpawnParameters SpawnParams;
//		SpawnParams.Owner = this;
//		SpawnParams.Instigator = GetInstigator();
//
//		// 在枪口位置生成发射物。
//		AYProjectile* Projectile = World->SpawnActor<AYProjectile>(ProjectileClass, MuzzleLocation, MuzzleRotation, SpawnParams);
//		if (Projectile)
//		{
//			// 设置发射物的初始轨迹。
//			FVector LaunchDirection = MuzzleRotation.Vector();
//			Projectile->FireInDirection(LaunchDirection);
//		}
//		else
//		{
//#if WITH_EDITOR
//			check(GEngine != nullptr);
//			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Failed to create projectile"));
//#endif
//		}
//	}
//}
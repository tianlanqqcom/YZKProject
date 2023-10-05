// Fill out your copyright notice in the Description page of Project Settings.


#include "YNFPPCharacter.h"
#include "Net/UnrealNetwork.h"
#include <string>


FString TouchPosToString(int32 X, int32 Y)
{
    std::string s = "";
    s += std::to_string(X);
    s += " ";
    s += std::to_string(Y);

    return FString(s.c_str());
}

AYNFPPCharacter::AYNFPPCharacter()
{
    // 将此角色设置为每帧调用Tick()。  如果不需要此特性，可以关闭以提升性能。
    PrimaryActorTick.bCanEverTick = true;

    // 创建第一人称摄像机组件。
    FPSCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
    check(FPSCameraComponent != nullptr);

    // 将摄像机组件附加到我们的胶囊体组件。
    FPSCameraComponent->SetupAttachment(CastChecked<USceneComponent>(GetCapsuleComponent()));

    // 将摄像机置于略高于眼睛上方的位置。
    FPSCameraComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f + BaseEyeHeight));

    // 启用Pawn控制摄像机旋转。
    FPSCameraComponent->bUsePawnControlRotation = true;

    // 为所属玩家创建第一人称网格体组件。
    FPSMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonMesh"));
    check(FPSMesh != nullptr);

    // 仅所属玩家可以看见此网格体。
    FPSMesh->SetOnlyOwnerSee(true);

    // 将 FPS 网格体附加到 FPS 摄像机。
    FPSMesh->SetupAttachment(FPSCameraComponent);

    // 禁用某些环境阴影以便实现只有单个网格体的感觉。
    FPSMesh->bCastDynamicShadow = false;
    FPSMesh->CastShadow = false;

    // 所属玩家看不到常规（第三人称）全身网格体。
    GetMesh()->SetOwnerNoSee(true);
}

void AYNFPPCharacter::GetLifetimeReplicatedProps(TArray <FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    //复制当前生命值。
    DOREPLIFETIME(AYNFPPCharacter, HealthPoint);
}

void AYNFPPCharacter::SetCurrentHealth(float healthValue)
{
    if (GetLocalRole() == ROLE_Authority)
    {
        HealthPoint = FMath::Clamp(healthValue, 0.f, HealthPoint);
        OnHealthUpdate();
    }
}

float AYNFPPCharacter::TakeDamage(float DamageTaken, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    float damageApplied = HealthPoint - DamageTaken;
    SetCurrentHealth(damageApplied);
    return damageApplied;
}

// 当游戏开始或重生（Spawn）时被调用
void AYNFPPCharacter::BeginPlay()
{
    Super::BeginPlay();

#if WITH_EDITOR
    check(GEngine != nullptr);

    // 显示调试消息五秒。 
    // -1"键"值参数可以防止更新或刷新消息。
    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("We are using FPSCharacter."));
#endif

}

// 每一帧都被调用
void AYNFPPCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    TimeAgainstLastFire += DeltaTime;

    // StartFire();

    TryFire();
}

// 被调用，将功能与输入绑定
void AYNFPPCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    // 设置"移动"绑定。
    PlayerInputComponent->BindAxis("MoveForward", this, &AYNFPPCharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &AYNFPPCharacter::MoveRight);

    // 设置"观看"绑定。
    PlayerInputComponent->BindAxis("Turn", this, &AYNFPPCharacter::AddControllerYawInput);
    PlayerInputComponent->BindAxis("LookUp", this, &AYNFPPCharacter::AddControllerPitchInput);

    // 设置"操作"绑定。
    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AYNFPPCharacter::StartJump);
    PlayerInputComponent->BindAction("Jump", IE_Released, this, &AYNFPPCharacter::StopJump);

    // 设置"开火"绑定。
    PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AYNFPPCharacter::Fire);

    PlayerInputComponent->BindTouch(IE_Pressed, this, &AYNFPPCharacter::OnTouchPressed);
    PlayerInputComponent->BindTouch(IE_Released, this, &AYNFPPCharacter::OnTouchReleased);
    PlayerInputComponent->BindTouch(IE_Repeat, this, &AYNFPPCharacter::OnTouchMoved);
}

void AYNFPPCharacter::MoveForward(float Value)
{
    // 找出"前进"方向，并记录玩家想向该方向移动。
    FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::X);
    AddMovementInput(Direction, Value);
}

void AYNFPPCharacter::MoveRight(float Value)
{
    // 找出"右侧"方向，并记录玩家想向该方向移动。
    FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::Y);
    AddMovementInput(Direction, Value);
}

void AYNFPPCharacter::StartJump()
{
    bPressedJump = true;
}

void AYNFPPCharacter::StopJump()
{
    bPressedJump = false;
}

void AYNFPPCharacter::Fire_Implementation()
{
    // 试图发射发射物。
    if (ProjectileClass)
    {
        // 获取摄像机变换。
        FVector CameraLocation;
        FRotator CameraRotation;
        GetActorEyesViewPoint(CameraLocation, CameraRotation);

        // 设置MuzzleOffset，在略靠近摄像机前生成发射物。
        // MuzzleOffset.Set(100.0f, 0.0f, 0.0f);

        // 将MuzzleOffset从摄像机空间变换到世界空间。
        FVector MuzzleLocation = CameraLocation + FTransform(CameraRotation).TransformVector(MuzzleOffset);

        // 使目标方向略向上倾斜。 
        FRotator MuzzleRotation = CameraRotation;
        MuzzleRotation.Pitch += WeaponPitchMuzzle;

        UWorld* World = GetWorld();
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
                Projectile->Firer = this;
                Projectile->FireInDirection(LaunchDirection);
            }

        }
    }
}

void AYNFPPCharacter::OnTouchPressed(ETouchIndex::Type FingerIndex, FVector Location)
{
    int32 ViewX, ViewY;
    GetWorld()->GetFirstPlayerController()->GetViewportSize(ViewX, ViewY);

#if WITH_EDITOR

    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TouchPosToString(Location.X, Location.Y));

#endif
    if (Location.X > (ViewX / 2))
    {
        bIsRotating = true;
        PreviousLocation = Location;
    }
}

void AYNFPPCharacter::OnTouchReleased(ETouchIndex::Type FingerIndex, FVector Location)
{
    bIsRotating = false;
}

void AYNFPPCharacter::OnTouchMoved(ETouchIndex::Type FingerIndex, FVector Location)
{
    int32 ViewX, ViewY;
    GetWorld()->GetFirstPlayerController()->GetViewportSize(ViewX, ViewY);
    if (bIsRotating && Location.X > (ViewX / 2))
    {
        // 根据手指滑动的距离来旋转镜头
        float DeltaX = Location.X - PreviousLocation.X;
        AddControllerYawInput(DeltaX * Sensitivity);

        float DeltaY = Location.Y - PreviousLocation.Y;
        AddControllerPitchInput(DeltaY * Sensitivity);
    }
    PreviousLocation = Location;
}

void AYNFPPCharacter::StartFire()
{
    this->bIsFiring = true;
}

void AYNFPPCharacter::TryFire()
{
    if (this->bIsFiring)
    {
        if (TimeAgainstLastFire > MinTimeDistanceOfFire)
        {
            Fire();
            TimeAgainstLastFire = 0.0f;
        }
    }
}

void AYNFPPCharacter::StopFire()
{
    this->bIsFiring = false;
}

void AYNFPPCharacter::OnRep_CurrentHealth()
{
    OnHealthUpdate();
}

void AYNFPPCharacter::OnHealthUpdate()
{
	//客户端特定的功能
	if (IsLocallyControlled())
	{
		FString healthMessage = FString::Printf(TEXT("You now have %f health remaining."), HealthPoint);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, healthMessage);

		if (HealthPoint <= 0)
		{
			FString deathMessage = FString::Printf(TEXT("You have been killed."));
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, deathMessage);
		}
	}

	//服务器特定的功能
	if (GetLocalRole() == ROLE_Authority)
	{
		FString healthMessage = FString::Printf(TEXT("%s now has %f health remaining."), *GetFName().ToString(), HealthPoint);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, healthMessage);
	}

	//在所有机器上都执行的函数。 
	/*
		因任何因伤害或死亡而产生的特殊功能都应放在这里。
	*/
    
}

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
    // ���˽�ɫ����Ϊÿ֡����Tick()��  �������Ҫ�����ԣ����Թر����������ܡ�
    PrimaryActorTick.bCanEverTick = true;

    // ������һ�˳�����������
    FPSCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
    check(FPSCameraComponent != nullptr);

    // �������������ӵ����ǵĽ����������
    FPSCameraComponent->SetupAttachment(CastChecked<USceneComponent>(GetCapsuleComponent()));

    // ������������Ը����۾��Ϸ���λ�á�
    FPSCameraComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f + BaseEyeHeight));

    // ����Pawn�����������ת��
    FPSCameraComponent->bUsePawnControlRotation = true;

    // Ϊ������Ҵ�����һ�˳������������
    FPSMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonMesh"));
    check(FPSMesh != nullptr);

    // ��������ҿ��Կ����������塣
    FPSMesh->SetOnlyOwnerSee(true);

    // �� FPS �����帽�ӵ� FPS �������
    FPSMesh->SetupAttachment(FPSCameraComponent);

    // ����ĳЩ������Ӱ�Ա�ʵ��ֻ�е���������ĸо���
    FPSMesh->bCastDynamicShadow = false;
    FPSMesh->CastShadow = false;

    // ������ҿ��������棨�����˳ƣ�ȫ�������塣
    GetMesh()->SetOwnerNoSee(true);
}

void AYNFPPCharacter::GetLifetimeReplicatedProps(TArray <FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    //���Ƶ�ǰ����ֵ��
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

// ����Ϸ��ʼ��������Spawn��ʱ������
void AYNFPPCharacter::BeginPlay()
{
    Super::BeginPlay();

#if WITH_EDITOR
    check(GEngine != nullptr);

    // ��ʾ������Ϣ���롣 
    // -1"��"ֵ�������Է�ֹ���»�ˢ����Ϣ��
    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("We are using FPSCharacter."));
#endif

}

// ÿһ֡��������
void AYNFPPCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    TimeAgainstLastFire += DeltaTime;

    // StartFire();

    TryFire();
}

// �����ã��������������
void AYNFPPCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    // ����"�ƶ�"�󶨡�
    PlayerInputComponent->BindAxis("MoveForward", this, &AYNFPPCharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &AYNFPPCharacter::MoveRight);

    // ����"�ۿ�"�󶨡�
    PlayerInputComponent->BindAxis("Turn", this, &AYNFPPCharacter::AddControllerYawInput);
    PlayerInputComponent->BindAxis("LookUp", this, &AYNFPPCharacter::AddControllerPitchInput);

    // ����"����"�󶨡�
    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AYNFPPCharacter::StartJump);
    PlayerInputComponent->BindAction("Jump", IE_Released, this, &AYNFPPCharacter::StopJump);

    // ����"����"�󶨡�
    PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AYNFPPCharacter::Fire);

    PlayerInputComponent->BindTouch(IE_Pressed, this, &AYNFPPCharacter::OnTouchPressed);
    PlayerInputComponent->BindTouch(IE_Released, this, &AYNFPPCharacter::OnTouchReleased);
    PlayerInputComponent->BindTouch(IE_Repeat, this, &AYNFPPCharacter::OnTouchMoved);
}

void AYNFPPCharacter::MoveForward(float Value)
{
    // �ҳ�"ǰ��"���򣬲���¼�������÷����ƶ���
    FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::X);
    AddMovementInput(Direction, Value);
}

void AYNFPPCharacter::MoveRight(float Value)
{
    // �ҳ�"�Ҳ�"���򣬲���¼�������÷����ƶ���
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
    // ��ͼ���䷢���
    if (ProjectileClass)
    {
        // ��ȡ������任��
        FVector CameraLocation;
        FRotator CameraRotation;
        GetActorEyesViewPoint(CameraLocation, CameraRotation);

        // ����MuzzleOffset�����Կ��������ǰ���ɷ����
        // MuzzleOffset.Set(100.0f, 0.0f, 0.0f);

        // ��MuzzleOffset��������ռ�任������ռ䡣
        FVector MuzzleLocation = CameraLocation + FTransform(CameraRotation).TransformVector(MuzzleOffset);

        // ʹĿ�귽����������б�� 
        FRotator MuzzleRotation = CameraRotation;
        MuzzleRotation.Pitch += WeaponPitchMuzzle;

        UWorld* World = GetWorld();
        if (World)
        {
            FActorSpawnParameters SpawnParams;
            SpawnParams.Owner = this;
            SpawnParams.Instigator = GetInstigator();

            // ��ǹ��λ�����ɷ����
            AYProjectile* Projectile = World->SpawnActor<AYProjectile>(ProjectileClass, MuzzleLocation, MuzzleRotation, SpawnParams);
            if (Projectile)
            {
                // ���÷�����ĳ�ʼ�켣��
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
        // ������ָ�����ľ�������ת��ͷ
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
	//�ͻ����ض��Ĺ���
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

	//�������ض��Ĺ���
	if (GetLocalRole() == ROLE_Authority)
	{
		FString healthMessage = FString::Printf(TEXT("%s now has %f health remaining."), *GetFName().ToString(), HealthPoint);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, healthMessage);
	}

	//�����л����϶�ִ�еĺ����� 
	/*
		���κ����˺������������������⹦�ܶ�Ӧ�������
	*/
    
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "YNFPPCharacter.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/PlayerController.h"
#include <string>
#include "YFPPRespawnGameMode.h"


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

	SetReplicates(true);
	FPSMesh->SetIsReplicated(true);
	// SetReplicateMovement(true);

}

void AYNFPPCharacter::GetLifetimeReplicatedProps(TArray <FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//���Ƶ�ǰ����ֵ��
	DOREPLIFETIME(AYNFPPCharacter, HealthPoint);

	DOREPLIFETIME(AYNFPPCharacter, Score);

	DOREPLIFETIME(AYNFPPCharacter, LastHitPerson);

	DOREPLIFETIME(AYNFPPCharacter, Name);
}

void AYNFPPCharacter::SetCurrentHealth(float healthValue)
{
	// if (GetLocalRole() == ROLE_Authority)
	{
		HealthPoint = FMath::Clamp(healthValue, 0.f, HealthPoint);
		OnHealthUpdate();
	}
}

void AYNFPPCharacter::SetCurrentScore(int32 NewScore)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		this->Score = NewScore;
		OnScoreUpdate();
	}
}

void AYNFPPCharacter::SetLastHitPerson(AYNFPPCharacter* NewLastHitPerson)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		this->LastHitPerson = NewLastHitPerson;
		OnLastHitPersonUpdate();
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

	int32 ViewX, ViewY;
	APlayerController* MyController = GetWorld()->GetFirstPlayerController();
	if(MyController)
	{
		MyController->GetViewportSize(ViewX, ViewY);
		TouchProcesser = FPlayerTouchProcesser(ViewX, ViewY);
	}
}

void AYNFPPCharacter::Destroyed()
{
	Super::Destroyed();

	//���ڰ���GameMode�е�OnPlayerDied�¼���ʾ�� 
	if (UWorld* World = GetWorld())
	{
		if (AYFPPRespawnGameMode* GameMode = Cast<AYFPPRespawnGameMode>(World->GetAuthGameMode()))
		{
			GameMode->GetOnPlayerDied().Broadcast(this);
		}
	}
}

void AYNFPPCharacter::CallRestartPlayer()
{
	// ���Pawn�����������á�
	AController* ControllerRef = GetController();
	bEnableUMGInput = true;

	// �������  
	// Destroy();

	//�������л��World��GameMode���Ե�����������Һ�����
	if (UWorld* World = GetWorld())
	{
		if (AYFPPRespawnGameMode* GameMode = Cast<AYFPPRespawnGameMode>(World->GetAuthGameMode()))
		{
			GameMode->RestartPlayerByYZK(ControllerRef);
		}

		if(APlayerController* PController = Cast<APlayerController>(this->GetController()))
		{
			EnableInput(PController);
		}
	}

}

void AYNFPPCharacter::RespawnTimeDelay()
{
#if WITH_EDITOR
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, L"Call RespawnTimeDelay");
#endif 
	// SetCurrentHealth(1.0f);
	HealthPoint = 1.0f;
	bCharacterHasDied = false;
	CallRestartPlayer();
	GetWorld()->GetTimerManager().ClearTimer(this->RespawnDelayHandle);
}

// ÿһ֡��������
void AYNFPPCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// ��ȡ�����ת�Ƕ�
	FRotator CameraRotation = GetControlRotation();

	// ֻ�����Լ����ڿ��ƵĽ�ɫ
	if (GetLocalRole() == ROLE_Authority || GetLocalRole() == ROLE_AutonomousProxy)
	{
#if WITH_EDITOR
		// GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, CameraRotation.ToString());
#endif

		// Pre-Process Pitch
		double CameraPitch = CameraRotation.Pitch;
		if (CameraPitch > 180.0)
		{
			CameraPitch -= 360.0;
		}

		// ���Ƹ�������+-70��֮��
		CameraPitch = FMath::Clamp(CameraPitch, -70, 70);

		// Post Process Pitch
		if (CameraPitch < 0)
		{
			CameraPitch += 360.0;
		}

		CameraRotation.Pitch = CameraPitch;

		FRotator FPSMeshRotator(CameraPitch - RecordLastPitch, 0, 0);
		RecordLastPitch = CameraPitch;

		// ���������ת�Ƕ�
		GetController()->SetControlRotation(CameraRotation);

		// FPSCameraComponent->SetWorldRotation();

		// FPSCameraComponent->AddLocalRotation(FPSMeshRotator);
	}


	TimeAgainstLastFire += DeltaTime;

	// StartFire();

	TryFire();

	// Debug
	FRotator NowRotator;
	FVector Unused;
	GetActorEyesViewPoint(Unused, NowRotator);

	FVector lastRotatorVec = RecordLastCameraRotator.Vector();
	if(abs(lastRotatorVec.Y - NowRotator.Vector().Y) > 90.0)
	{
#if WITH_EDITOR
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Purple, FString::Printf(L"Now RotatorY: %lf, Last: %lf, bIsRota:%d",
			lastRotatorVec.Y, RecordLastCameraRotator.Vector().Y, bIsRotating));
#endif

	}
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

	// Respawn Actor Debug Action: Key G
	PlayerInputComponent->BindAction("Restart", IE_Pressed, this, &AYNFPPCharacter::CallRestartPlayer);
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
	/*int32 ViewX, ViewY;
	GetWorld()->GetFirstPlayerController()->GetViewportSize(ViewX, ViewY);

#if WITH_EDITOR

	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TouchPosToString(Location.X, Location.Y));

#endif
	if (Location.X > (ViewX / 2))
	{
		bIsRotating = true;
		PreviousLocation = Location;
	}
	else
	{
		bIsRotating = false;
	}*/

	TouchProcesser.FingerPressed(FingerIndex, Location);
}

void AYNFPPCharacter::OnTouchReleased(ETouchIndex::Type FingerIndex, FVector Location)
{
	// bIsRotating = false;
	TouchProcesser.FingerRelease(FingerIndex, Location);
}

void AYNFPPCharacter::OnTouchMoved(ETouchIndex::Type FingerIndex, FVector Location)
{
	//int32 ViewX, ViewY;
	//GetWorld()->GetFirstPlayerController()->GetViewportSize(ViewX, ViewY);
	//if (bIsRotating && (FingerIndex == ETouchIndex::Touch1 || FingerIndex == ETouchIndex::Touch2) && Location.X > (ViewX / 2))
	//{
	//	// ������ָ�����ľ�������ת��ͷ
	//	float DeltaX = Location.X - PreviousLocation.X;
	//	AddControllerYawInput(DeltaX * Sensitivity);

	//	float DeltaY = Location.Y - PreviousLocation.Y;
	//	AddControllerPitchInput(DeltaY * Sensitivity);
	//}
	//PreviousLocation = Location;

	FTouchResult Result = TouchProcesser.FingerMoved(FingerIndex, Location);

	if(Result.DeltaX != 0.f)
	{
		AddControllerYawInput(Result.DeltaX * Sensitivity);
	}

	if (Result.DeltaY != 0.f)
	{
		AddControllerPitchInput(Result.DeltaY * Sensitivity);
	}
}

void AYNFPPCharacter::StartFire()
{
	this->bIsFiring = true;
}

void AYNFPPCharacter::TryFire()
{
	if (this->bIsFiring)
	{
		if (TimeAgainstLastFire > MinTimeDistanceOfFire && HealthPoint > 0.f)
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

void AYNFPPCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
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

		if (HealthPoint <= 0)
		{
			if(this->LastHitPerson && !bCharacterHasDied)
			{
				this->LastHitPerson->SetCurrentScore(this->LastHitPerson->Score + 1);
			}
			
			// Controller->DisableInput(nullptr);
			// CallRestartPlayer();
			bCharacterHasDied = true;
			GetWorld()->GetTimerManager().SetTimer(RespawnDelayHandle, this, &AYNFPPCharacter::RespawnTimeDelay, 3.0f, true);
		}
	}

	if(HealthPoint <= 0)
	{
		this->bIsFiring = false;
		this->bEnableUMGInput = false;

		if(APlayerController* PController = Cast<APlayerController>(this->GetController()))
		{
			DisableInput(PController);
		}
	}


	//�����л����϶�ִ�еĺ����� 
	/*
		���κ����˺������������������⹦�ܶ�Ӧ�������
	*/

}

void AYNFPPCharacter::OnScoreUpdate()
{
	//�ͻ����ض��Ĺ���
	if (IsLocallyControlled())
	{
		
	}

	//�������ض��Ĺ���
	if (GetLocalRole() == ROLE_Authority)
	{
		
	}

	//�����л����϶�ִ�еĺ����� 
	
}

void AYNFPPCharacter::OnLastHitPersonUpdate()
{
	//�ͻ����ض��Ĺ���
	if (IsLocallyControlled())
	{

	}

	//�������ض��Ĺ���
	if (GetLocalRole() == ROLE_Authority)
	{

	}

	//�����л����϶�ִ�еĺ����� 
}

void AYNFPPCharacter::OnNameUpdate()
{
	//�ͻ����ض��Ĺ���
	if (IsLocallyControlled())
	{

	}

	//�������ض��Ĺ���
	if (GetLocalRole() == ROLE_Authority)
	{

	}
}



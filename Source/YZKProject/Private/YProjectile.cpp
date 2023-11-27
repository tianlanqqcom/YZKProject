// Fill out your copyright notice in the Description page of Project Settings.


#include "YProjectile.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/DamageType.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "YNFPPCharacter.h"

// Sets default values
AYProjectile::AYProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    // Enable actor replicate.
    bReplicates = true;

    // 20 ���ɾ�������
    InitialLifeSpan = 20.0f;

    if (!RootComponent)
    {
        RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ProjectileSceneComponent"));
    }

    if (!CollisionComponent)
    {
        // ��������м򵥵���ײչʾ��
        CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
        // �����������ײ�뾶��
        CollisionComponent->InitSphereRadius(15.0f);
        // ���������ײ�����ļ���������Ϊ"Projectile"��
        CollisionComponent->BodyInstance.SetCollisionProfileName(TEXT("Projectile"));
        // �������ĳ��ʱ���õ��¼���
        CollisionComponent->OnComponentHit.AddDynamic(this, &AYProjectile::OnHit);
        // �����������Ϊ��ײ�����
        RootComponent = CollisionComponent;

        if (GetLocalRole() == ROLE_Authority)
        {
            CollisionComponent->OnComponentHit.AddDynamic(this, &AYProjectile::OnProjectileImpact);
        }
    }

    if (!ProjectileMovementComponent)
    {
        // ʹ�ô����������������ƶ���
        ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
        ProjectileMovementComponent->SetUpdatedComponent(CollisionComponent);
        // ProjectileMovementComponent->InitialSpeed = 3000.0f;
        ProjectileMovementComponent->InitialSpeed = 3000.0f;
        ProjectileMovementComponent->MaxSpeed = 3000.0f;
        ProjectileMovementComponent->bRotationFollowsVelocity = true;
        ProjectileMovementComponent->bShouldBounce = true;
        ProjectileMovementComponent->Bounciness = 0.3f;
        ProjectileMovementComponent->ProjectileGravityScale = 0.02f;
    }

    if (!ProjectileMeshComponent)
    {
        ProjectileMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMeshComponent"));
        static ConstructorHelpers::FObjectFinder<UStaticMesh>Mesh(TEXT("'/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere'"));
        if (Mesh.Succeeded())
        {
            ProjectileMeshComponent->SetStaticMesh(Mesh.Object);
        }
    }

    static ConstructorHelpers::FObjectFinder<UMaterial>Material(TEXT("'/Game/StarterContent/Materials/M_Basic_Floor.M_Basic_Floor'"));
    if (Material.Succeeded())
    {
        ProjectileMaterialInstance = UMaterialInstanceDynamic::Create(Material.Object, ProjectileMeshComponent);
    }
    ProjectileMeshComponent->SetMaterial(0, ProjectileMaterialInstance);
    ProjectileMeshComponent->SetRelativeScale3D(FVector(0.025f, 0.025f, 0.025f));
    ProjectileMeshComponent->SetupAttachment(RootComponent);

    static ConstructorHelpers::FObjectFinder<UParticleSystem> DefaultExplosionEffect(TEXT("/Game/StarterContent/Particles/P_Explosion.P_Explosion"));
    if (DefaultExplosionEffect.Succeeded())
    {
        ExplosionEffect = DefaultExplosionEffect.Object;
    }

    // Init damage
    DamageType = UDamageType::StaticClass();
    // Damage = 10.0f;
    Damage = FMath::RandRange(0.05f, 0.3f);
}

// Called when the game starts or when spawned
void AYProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

void AYProjectile::OnProjectileImpact(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if(Cast<AYNFPPCharacter>(OtherActor) == Firer)
    {
	    return;
    }

    if (auto PersonGetHit = Cast<AYNFPPCharacter>(OtherActor))
    {
        PersonGetHit->SetLastHitPerson(Firer);
        PersonGetHit->SetCurrentHealth(PersonGetHit->GetCurrentHealth() - Damage);
    }

    if (OtherActor)
    {
        UGameplayStatics::ApplyPointDamage(OtherActor, Damage, NormalImpulse, Hit, GetInstigator()->Controller, this, DamageType);
    }

    Destroy();
}

// Called every frame
void AYProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AYProjectile::OnProjectileBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OterComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit)
{
    if (Cast<AYNFPPCharacter>(OtherActor) == Firer)
    {
        return;
    }

    auto PersonGetHit = Cast<AYNFPPCharacter>(OtherActor);
    if(PersonGetHit)
    {
        PersonGetHit->SetLastHitPerson(Firer);
        PersonGetHit->SetCurrentHealth(PersonGetHit->GetCurrentHealth() - Damage);
    }

    if (OtherActor)
    {
        UGameplayStatics::ApplyPointDamage(OtherActor, Damage, FVector(0,0,0), Hit, GetInstigator()->Controller, this, DamageType);
    }

    Destroy();
}

// ��ʼ����������Ϸ������ٶȵĺ�����
void AYProjectile::FireInDirection(const FVector& ShootDirection)
{
    ProjectileMovementComponent->Velocity = ShootDirection * ProjectileMovementComponent->InitialSpeed;
}

void AYProjectile::Destroyed()
{
    Super::Destroyed();
    FVector SpawnLocation = GetActorLocation();
    FRotator SpawnRotator = FRotator::ZeroRotator;
    FVector SpawnScale = FVector(0.1, 0.1, 0.1);
    FTransform SpawnTransform(SpawnRotator, SpawnLocation, SpawnScale);

    if(ExplosionEffect)
    {
        // UParticleSystemComponent* ParticleSystemComponent = UGameplayStatics::SpawnEmitterAtLocation(this, ExplosionEffect, spawnLocation, FRotator::ZeroRotator, true, EPSCPoolMethod::AutoRelease);
        UParticleSystemComponent* ParticleSystemComponent = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, SpawnTransform, true, EPSCPoolMethod::AutoRelease, true);
    	// ParticleSystemComponent->SetWorldScale3D(FVector(0.1f, 0.1f, 0.1f));
    }

}

// ���������������ʱ����õĺ�����
void AYProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
    // Disable old codes.
#ifdef YZK_LAST_PROJ
    AYNFPPCharacter* Character = Cast<AYNFPPCharacter>(OtherActor);
    if (Character)
    {
        float Damage = FMath::RandRange(0.05f, 1.0f);
#if WITH_EDITOR
        GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, FString((("Damage: " + std::to_string(Damage)).c_str())));
        GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, FString(("Before: " + std::to_string(Character->HealthPoint)).c_str()));
#endif

        // Character->TakeDamage(Damage);
#if WITH_EDITOR
        GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, FString(("After: " + std::to_string(Character->HealthPoint)).c_str()));
#endif
    }

//    if (OtherActor != this && OtherComponent->IsSimulatingPhysics())
//    {
//        OtherComponent->AddImpulseAtLocation(ProjectileMovementComponent->Velocity * 100.0f, Hit.ImpactPoint);
//    }
//#if WITH_EDITOR
//    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, L"Destroy on Hit");
//
//#endif

    Destroy();
#endif

}


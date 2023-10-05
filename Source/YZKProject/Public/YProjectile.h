// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "YProjectile.generated.h"

UCLASS()
class YZKPROJECT_API AYProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AYProjectile();

	class AYNFPPCharacter* Firer;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(Category = "Projectile")
	void OnProjectileImpact(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void OnProjectileBeginOverlap(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,    //����ʱ������壬ͨ���ǿ��Ƶ���
		UPrimitiveComponent* OterComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& Hit);


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// ���������������ʱ����õĺ�����
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

	// ������ײ���
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	class USphereComponent* CollisionComponent;

	// �������ƶ����
	UPROPERTY(VisibleAnywhere, Category = Movement)
	class UProjectileMovementComponent* ProjectileMovementComponent;

	// ������������
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	class UStaticMeshComponent* ProjectileMeshComponent;

	// ���������
	UPROPERTY(VisibleDefaultsOnly, Category = Movement)
	class UMaterialInstanceDynamic* ProjectileMaterialInstance;

	// ��Ͷ����ײ���������󲢱�ըʱʹ�õ����ӡ�
	UPROPERTY(EditAnywhere, Category = "Effects")
	class UParticleSystem* ExplosionEffect;

	//��Ͷ���ｫ��ɵ��˺����ͺ��˺���
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
	TSubclassOf<class UDamageType> DamageType;

	//��Ͷ������ɵ��˺���
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
	float Damage;

	void FireInDirection(const FVector& ShootDirection);

	virtual void Destroyed() override;

};

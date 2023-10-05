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
		AActor* OtherActor,    //触发时间的主体，通常是控制的人
		UPrimitiveComponent* OterComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& Hit);


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// 当发射物击中物体时会调用的函数。
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

	// 球体碰撞组件
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	class USphereComponent* CollisionComponent;

	// 发射物移动组件
	UPROPERTY(VisibleAnywhere, Category = Movement)
	class UProjectileMovementComponent* ProjectileMovementComponent;

	// 发射物网格体
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	class UStaticMeshComponent* ProjectileMeshComponent;

	// 发射物材质
	UPROPERTY(VisibleDefaultsOnly, Category = Movement)
	class UMaterialInstanceDynamic* ProjectileMaterialInstance;

	// 在投射物撞击其他对象并爆炸时使用的粒子。
	UPROPERTY(EditAnywhere, Category = "Effects")
	class UParticleSystem* ExplosionEffect;

	//此投射物将造成的伤害类型和伤害。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
	TSubclassOf<class UDamageType> DamageType;

	//此投射物造成的伤害。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
	float Damage;

	void FireInDirection(const FVector& ShootDirection);

	virtual void Destroyed() override;

};

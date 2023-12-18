 // Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS()
class BLASTER_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AProjectile();

protected:
	virtual void BeginPlay() override;
	void SpawnTrailSystem();
	void StartDestroyTimer();
	void DestroyTimerFinished();
	void ExplodeDamage();

	// DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE_FiveParams( FComponentHitSignature, UPrimitiveComponent, 
	// OnComponentHit, UPrimitiveComponent*, HitComponent, AActor*, OtherActor, UPrimitiveComponent*, OtherComp, 
	// FVector, NormalImpulse, const FHitResult&, Hit );
	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent *HitComp, AActor *OtherActor, UPrimitiveComponent *OtherComp, FVector NormalImpulse, const FHitResult &Hit);

	// 伤害
	UPROPERTY(EditAnywhere)
	float Damage = 20.f;

	// 粒子效果
	UPROPERTY(EditAnywhere)
	class UParticleSystem *ImpactParticles;

	// 音效
	UPROPERTY(EditAnywhere)
	class USoundCue *ImpactSound;

	UPROPERTY(EditAnywhere)
	class UBoxComponent *CollisionBox;

	UPROPERTY(VisibleAnywhere)
	class UProjectileMovementComponent *ProjectileMovementComponent;

	UPROPERTY(EditAnywhere)
	class UNiagaraSystem *TrailSystem;

	UPROPERTY()
	class UNiagaraComponent *TrailSystemComponent;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent *ProjectileMesh;

	// 爆炸类子弹的爆炸半径
	UPROPERTY(EditAnywhere)
	float DamageInnerRadius = 200.f;

	UPROPERTY(EditAnywhere)
	float DamageOuterRadius = 500.f;

private:
	UPROPERTY(EditAnywhere)
	UParticleSystem *Tracer;

	UPROPERTY()
	class UParticleSystemComponent *TracerComponent;

	/*
	* 让尾迹在一段时间后消失
	*/
	FTimerHandle DestroyTimer;

	UPROPERTY(EditAnywhere)
	float DestroyTime = 3.f;
	
public:	
	virtual void Tick(float DeltaTime) override;
	virtual void Destroyed() override;

};

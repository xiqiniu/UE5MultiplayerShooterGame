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

	// 是否开启SSR
	bool bUseServerSideRewind = false;

	FVector_NetQuantize TraceStart;
	// InitialVelocity包含方向的信息,使用NetQuantize100更为精确
	FVector_NetQuantize100 InitialVelocity;
	
	UPROPERTY(EditAnywhere)
	float InitialSpeed = 15000.f;

	// 只用于设置手雷和火箭的伤害
	UPROPERTY(EditAnywhere)
	float Damage = 20.f;

	// 对手雷和火箭不起作用
	UPROPERTY(EditAnywhere)
	float HeadShotDamage = 40.f;

protected:
	virtual void BeginPlay() override;
	void SpawnTrailSystem();
	void StartDestroyTimer();
	void DestroyTimerFinished();
	void ExplodeDamage();

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent *HitComp, AActor *OtherActor, UPrimitiveComponent *OtherComp, FVector NormalImpulse, const FHitResult &Hit);

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

	// 子弹尾迹(用于火箭和榴弹)
	UPROPERTY(EditAnywhere)
	class UNiagaraSystem *TrailSystem;

	UPROPERTY()
	class UNiagaraComponent *TrailSystemComponent;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent *ProjectileMesh;
	
	// 让尾迹在一段时间后消失
	FTimerHandle DestroyTimer;

	UPROPERTY(EditAnywhere)
	float DestroyTime = 3.f;

	// 爆炸类子弹的爆炸半径
	UPROPERTY(EditAnywhere)
	float DamageInnerRadius = 200.f;

	UPROPERTY(EditAnywhere)
	float DamageOuterRadius = 500.f;

private:
	// 用于发射子弹的武器显示子弹轨迹
	UPROPERTY(EditAnywhere)
	UParticleSystem *Tracer;

	UPROPERTY()
	class UParticleSystemComponent *TracerComponent;

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void Destroyed() override;
};

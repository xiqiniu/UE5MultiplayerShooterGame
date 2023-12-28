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

	// �Ƿ���SSR
	bool bUseServerSideRewind = false;

	FVector_NetQuantize TraceStart;
	// InitialVelocity�����������Ϣ,ʹ��NetQuantize100��Ϊ��ȷ
	FVector_NetQuantize100 InitialVelocity;
	
	UPROPERTY(EditAnywhere)
	float InitialSpeed = 15000.f;

	// ֻ�����������׺ͻ�����˺�
	UPROPERTY(EditAnywhere)
	float Damage = 20.f;

	// �����׺ͻ����������
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

	// ����Ч��
	UPROPERTY(EditAnywhere)
	class UParticleSystem *ImpactParticles;

	// ��Ч
	UPROPERTY(EditAnywhere)
	class USoundCue *ImpactSound;

	UPROPERTY(EditAnywhere)
	class UBoxComponent *CollisionBox;

	UPROPERTY(VisibleAnywhere)
	class UProjectileMovementComponent *ProjectileMovementComponent;

	// �ӵ�β��(���ڻ������)
	UPROPERTY(EditAnywhere)
	class UNiagaraSystem *TrailSystem;

	UPROPERTY()
	class UNiagaraComponent *TrailSystemComponent;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent *ProjectileMesh;
	
	// ��β����һ��ʱ�����ʧ
	FTimerHandle DestroyTimer;

	UPROPERTY(EditAnywhere)
	float DestroyTime = 3.f;

	// ��ը���ӵ��ı�ը�뾶
	UPROPERTY(EditAnywhere)
	float DamageInnerRadius = 200.f;

	UPROPERTY(EditAnywhere)
	float DamageOuterRadius = 500.f;

private:
	// ���ڷ����ӵ���������ʾ�ӵ��켣
	UPROPERTY(EditAnywhere)
	UParticleSystem *Tracer;

	UPROPERTY()
	class UParticleSystemComponent *TracerComponent;

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void Destroyed() override;
};

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

	// DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE_FiveParams( FComponentHitSignature, UPrimitiveComponent, 
	// OnComponentHit, UPrimitiveComponent*, HitComponent, AActor*, OtherActor, UPrimitiveComponent*, OtherComp, 
	// FVector, NormalImpulse, const FHitResult&, Hit );
	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent *HitComp, AActor *OtherActor, UPrimitiveComponent *OtherComp, FVector NormalImpulse, const FHitResult &Hit);

	// �˺�
	UPROPERTY(EditAnywhere)
	float Damage = 20.f;

private:
	UPROPERTY(EditAnywhere)
	class UBoxComponent *CollisionBox;

	UPROPERTY(VisibleAnywhere)
	class UProjectileMovementComponent *ProjectileMovementComponent;

	UPROPERTY(EditAnywhere)
	class UParticleSystem *Tracer;

	UPROPERTY()
	class UParticleSystemComponent *TracerComponent;
	
	// ����Ч��
	UPROPERTY(EditAnywhere)
	UParticleSystem *ImpactParticles;

	// ��Ч
	UPROPERTY(EditAnywhere)
	class USoundCue *ImpactSound;

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void Destroyed() override;

};

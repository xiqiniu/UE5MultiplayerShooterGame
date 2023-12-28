// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "HitScanWeapon.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API AHitScanWeapon : public AWeapon
{
	GENERATED_BODY()
	
public:
	virtual void Fire(const FVector &HitTarget) override;

protected:
	void WeaponTraceHit(const FVector &TraceStart, const FVector &HitTarget, FHitResult &OutHit);

	UPROPERTY(EditAnywhere)
	class UParticleSystem *ImpactParticles;

	UPROPERTY(EditAnywhere)
	USoundCue *HitSound;

private:
	UPROPERTY(EditAnywhere)
	UParticleSystem *BeamParticles;

	// SMG没有开火动画,所以要自己生成枪口的火花
	UPROPERTY(EditAnywhere)
	UParticleSystem *MuzzleFlash;

	UPROPERTY(EditAnywhere)
	USoundCue *FireSound;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "Flag.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API AFlag : public AWeapon
{
	GENERATED_BODY()
public:
	AFlag();
	virtual void Dropped() override;
	void ResetFlag();
protected:
	virtual void OnEquipped() override;
	virtual void OnDropped() override;
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent *FlagMesh;

	// ��¼���ӳ�ʼλ��,����Ż�ȥ
	FTransform InitialTransform;

public:
	FORCEINLINE FTransform GetInitialLocation() const { return InitialTransform; }

};

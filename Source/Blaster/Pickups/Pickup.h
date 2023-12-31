// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Pickup.generated.h"

/*
* 道具
*/
UCLASS()
class BLASTER_API APickup : public AActor
{
	GENERATED_BODY()
	
public:	
	APickup();
	virtual void Tick(float DeltaTime) override;
	virtual void Destroyed() override;

protected:
	UPROPERTY(EditAnywhere)
	float BaseTurnRate = 45.f;

	virtual void BeginPlay() override;

	// 设置为虚函数,方便不同道具实现不同效果
	UFUNCTION()
	virtual void OnSphereOverlap(
		UPrimitiveComponent *OverlappedComponent, 
		AActor *OtherActor, 
		UPrimitiveComponent *OtherComp,
		int32 OtherBodyIndex, 
		bool bFromSweep, 
		const FHitResult &SweepResult
	);

private:
	UPROPERTY(EditAnywhere)
	class USphereComponent *OverlapSphere;

	UPROPERTY(EditAnywhere)
	class USoundCue *PickupSound;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent *PickupMesh;

	// 拾取时播放的特效
	UPROPERTY(VisibleAnywhere)
	class UNiagaraComponent *PickupEffectComponent;

	UPROPERTY(EditAnywhere)
	class UNiagaraSystem *PickupEffect;

	FTimerHandle BindOverlapTimer;

	float BindOverlapTime = 0.25f;
	void BindOverlapTimerFinished();
};

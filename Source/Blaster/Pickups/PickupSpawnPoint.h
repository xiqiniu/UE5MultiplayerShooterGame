// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickupSpawnPoint.generated.h"

/*
* 道具生成点
*/
UCLASS()
class BLASTER_API APickupSpawnPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	APickupSpawnPoint();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	// 设置能生成哪些道具
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<class APickup>> PickupClasses;

	UPROPERTY()
	APickup *SpawnedPickup;

	void SpawnPickup();
	
	void SpawnPickupTimerFinished();

	UFUNCTION()
	void StartPickupTimer(AActor * DestroyedActor);
private:
	FTimerHandle SpawnPickupTimer;

	// 道具被捡取后,过(SpawnPickupTimeMin, SpawnPickupTimeMax)后生成新的
	UPROPERTY(EditAnywhere)
	float SpawnPickupTimeMin;

	UPROPERTY(EditAnywhere)
	float SpawnPickupTimeMax;
};

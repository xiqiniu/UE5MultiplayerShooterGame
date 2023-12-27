// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickupSpawnPoint.generated.h"

/*
* �������ɵ�
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

	// ������������Щ����
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

	// ���߱���ȡ��,��(SpawnPickupTimeMin, SpawnPickupTimeMax)�������µ�
	UPROPERTY(EditAnywhere)
	float SpawnPickupTimeMin;

	UPROPERTY(EditAnywhere)
	float SpawnPickupTimeMax;
};

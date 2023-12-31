// Fill out your copyright notice in the Description page of Project Settings.


#include "PickupSpawnPoint.h"
#include "Pickup.h"
#include "TimerManager.h"
APickupSpawnPoint::APickupSpawnPoint()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}


void APickupSpawnPoint::BeginPlay()
{
	Super::BeginPlay();
	
	// 游戏开始时就生成道具
	if (HasAuthority())
	{
		SpawnPickup();
	}
}

void APickupSpawnPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APickupSpawnPoint::SpawnPickup()
{
	int32 NumOfPickupClasses = PickupClasses.Num();
	if (NumOfPickupClasses > 0)
	{
		int32 Selection = FMath::RandRange(0, NumOfPickupClasses - 1);
		// 记下生成的道具,被捡了就要重新生成
		SpawnedPickup = GetWorld()->SpawnActor<APickup>(
			PickupClasses[Selection],
			GetActorTransform()
		);

		// 生成的道具被拾取后开启定时器生成下一个道具
		if (HasAuthority() && SpawnedPickup)
		{
			SpawnedPickup->OnDestroyed.AddDynamic(this, &APickupSpawnPoint::StartPickupTimer);
		}
	}
}

void APickupSpawnPoint::StartPickupTimer(AActor *DestroyedActor)
{
	const float SpawnTime = FMath::RandRange(SpawnPickupTimeMin, SpawnPickupTimeMax);
	GetWorldTimerManager().SetTimer(
		SpawnPickupTimer,
		this,
		&APickupSpawnPoint::SpawnPickupTimerFinished,
		SpawnTime
	);
}

void APickupSpawnPoint::SpawnPickupTimerFinished()
{
	if (HasAuthority())
	{
		SpawnPickup();
	}
}




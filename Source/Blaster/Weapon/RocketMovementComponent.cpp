// Fill out your copyright notice in the Description page of Project Settings.


#include "RocketMovementComponent.h"

URocketMovementComponent::EHandleBlockingHitResult URocketMovementComponent::HandleBlockingHit(const FHitResult &Hit, float TimeTick, const FVector &MoveDelta, float &SubTickTimeRemaining)
{
	// 火箭不在中途停止,只在检测到碰撞的时候爆炸
	Super::HandleBlockingHit(Hit, TimeTick, MoveDelta, SubTickTimeRemaining);
	return EHandleBlockingHitResult::AdvanceNextSubstep;
}

void URocketMovementComponent::HandleImpact(const FHitResult &Hit, float TimeSlice, const FVector &MoveDelta)
{
	// 火箭不在中途停止,只在检测到碰撞的时候爆炸
}

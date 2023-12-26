// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "BlasterGameInstance.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSkipWarmupChange, bool, bShouldSkipWarmup);

UCLASS()
class BLASTER_API UBlasterGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	bool bShouldSkipWarmup = false;
	void SetShouldSkipWarmup(bool bSkipWarmup);
	FSkipWarmupChange OnSkipWarmupChanged;
};

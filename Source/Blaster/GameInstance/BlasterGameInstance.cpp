// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterGameInstance.h"

void UBlasterGameInstance::SetShouldSkipWarmup(bool bSkipWarmup)
{
	bShouldSkipWarmup = bSkipWarmup;
	OnSkipWarmupChanged.Broadcast(bSkipWarmup);
}

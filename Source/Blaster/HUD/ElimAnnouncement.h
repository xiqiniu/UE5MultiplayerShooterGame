// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ElimAnnouncement.generated.h"

/**
 * 击杀时显示的信息
 */
UCLASS()
class BLASTER_API UElimAnnouncement : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetElimAnnouncementText(FText AttackerName, FText VictimName);

	UPROPERTY(meta = (BindWidget))
	class UHorizontalBox *AnnouncementBox;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock *AnnouncementText;
};

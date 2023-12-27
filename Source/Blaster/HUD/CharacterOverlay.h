// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterOverlay.generated.h"

/**
 * 显示玩家信息: 血量,护盾,得分, 死亡,团队分数,弹药,手雷,高延迟图标
 */
UCLASS()
class BLASTER_API UCharacterOverlay : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta = (BindWidget))
	class UProgressBar *HealthBar;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock *HealthText;

	UPROPERTY(meta = (BindWidget))
	UProgressBar *ShieldBar;

	UPROPERTY(meta = (BindWidget))
	UTextBlock *ShieldText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock *ScoreAmount;

	UPROPERTY(meta = (BindWidget))
	UTextBlock *DefeatsAmount;

	UPROPERTY(meta = (BindWidget))
	UTextBlock *RedTeamScore;

	UPROPERTY(meta = (BindWidget))
	UTextBlock *BlueTeamScore;

	UPROPERTY(meta = (BindWidget))
	UTextBlock *ScoreSpacerText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock *WeaponAmmoAmount;

	UPROPERTY(meta = (BindWidget))
	UTextBlock *CarriedAmmoAmount;

	UPROPERTY(meta = (BindWidget))
	UTextBlock *MatchCountdownText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock *GrenadesText;

	UPROPERTY(meta = (BindWidget))
	class UImage *HighPingImage;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation *HighPingAnimation;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "BlasterGameState.generated.h"

/**
 * 游戏相关的变量: 当前领先的玩家及其分数, 队伍分数
 */
UCLASS()
class BLASTER_API ABlasterGameState : public AGameState
{
	GENERATED_BODY()
public:
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;
	void UpdateTopScore(class ABlasterPlayerState *ScoringPlayer);

	UPROPERTY(Replicated)
	TArray<ABlasterPlayerState *> TopScoringPlayers;

	float TopScore = 0.f;

	void RedTeamScores();
	void BlueTeamScores();
	TArray<ABlasterPlayerState *> RedTeam;
	TArray<ABlasterPlayerState *> BlueTeam;

	UPROPERTY(ReplicatedUsing = OnRep_RedTeamScore)
	float RedTeamScore = 0.f;

	UPROPERTY(ReplicatedUsing = OnRep_BlueTeamScore)
	float BlueTeamScore = 0.f;

	UFUNCTION()
	void OnRep_RedTeamScore();

	UFUNCTION()
	void OnRep_BlueTeamScore();
};

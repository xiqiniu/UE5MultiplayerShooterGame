// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BlasterGameMode.h"
#include "TeamsGameMode.generated.h"

/**
 * 团队竞技模式使用的模式
 */
UCLASS()
class BLASTER_API ATeamsGameMode : public ABlasterGameMode
{
	GENERATED_BODY()
public:
	ATeamsGameMode();
	// 重写PostLogin使玩家中途加入也能分到队伍
	virtual void PostLogin(APlayerController *NewPlayer) override;

	// 重写Logout使玩家退出时GameState能将其PlayerState移出对应的TArray
	virtual void Logout(AController *Exiting) override;

	// 重写CalculateDamage避免友军伤害
	virtual float CalculateDamage(AController *Attacker, AController *Victim, float BaseDamage) override;

	// 重写PlayerEliminated使得击杀时能增加团队分数
	virtual void PlayerEliminated(class ABlasterCharacter *ElimmedCharacter,
		class ABlasterPlayerController *VictimController, ABlasterPlayerController *AttackerController) override;

protected:
	// 重写HandleMatchHasStarted使MatchState转变为InProgress时(即游戏开始时)玩家进行分队
	virtual void HandleMatchHasStarted() override;

private:
	virtual void GameModeRestartGame() override;
};

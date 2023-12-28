// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TeamsGameMode.h"
#include "CaptureTheFlagGameMode.generated.h"

/**
 * 夺旗模式使用的模式
 */
UCLASS()
class BLASTER_API ACaptureTheFlagGameMode : public ATeamsGameMode
{
	GENERATED_BODY()
public:
	// 重写PlayerEliminated,因为夺旗模式继承自团队模式,不重写的话击杀的时候会增加团队分数
	virtual void PlayerEliminated(class ABlasterCharacter *ElimmedCharacter,
		class ABlasterPlayerController *VictimController, ABlasterPlayerController *AttackerController);

	// 抢夺对手的旗子到自己的领地后获得团队分数
	void FlagCaptured(class AFlag *Flag, class AFlagZone *FlagZone);

private:
	virtual void GameModeRestartGame() override;
};

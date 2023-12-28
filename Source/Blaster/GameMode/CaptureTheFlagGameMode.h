// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TeamsGameMode.h"
#include "CaptureTheFlagGameMode.generated.h"

/**
 * ����ģʽʹ�õ�ģʽ
 */
UCLASS()
class BLASTER_API ACaptureTheFlagGameMode : public ATeamsGameMode
{
	GENERATED_BODY()
public:
	// ��дPlayerEliminated,��Ϊ����ģʽ�̳����Ŷ�ģʽ,����д�Ļ���ɱ��ʱ��������Ŷӷ���
	virtual void PlayerEliminated(class ABlasterCharacter *ElimmedCharacter,
		class ABlasterPlayerController *VictimController, ABlasterPlayerController *AttackerController);

	// ������ֵ����ӵ��Լ�����غ����Ŷӷ���
	void FlagCaptured(class AFlag *Flag, class AFlagZone *FlagZone);

private:
	virtual void GameModeRestartGame() override;
};

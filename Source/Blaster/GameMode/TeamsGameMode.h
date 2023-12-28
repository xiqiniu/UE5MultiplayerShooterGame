// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BlasterGameMode.h"
#include "TeamsGameMode.generated.h"

/**
 * �ŶӾ���ģʽʹ�õ�ģʽ
 */
UCLASS()
class BLASTER_API ATeamsGameMode : public ABlasterGameMode
{
	GENERATED_BODY()
public:
	ATeamsGameMode();
	// ��дPostLoginʹ�����;����Ҳ�ֵܷ�����
	virtual void PostLogin(APlayerController *NewPlayer) override;

	// ��дLogoutʹ����˳�ʱGameState�ܽ���PlayerState�Ƴ���Ӧ��TArray
	virtual void Logout(AController *Exiting) override;

	// ��дCalculateDamage�����Ѿ��˺�
	virtual float CalculateDamage(AController *Attacker, AController *Victim, float BaseDamage) override;

	// ��дPlayerEliminatedʹ�û�ɱʱ�������Ŷӷ���
	virtual void PlayerEliminated(class ABlasterCharacter *ElimmedCharacter,
		class ABlasterPlayerController *VictimController, ABlasterPlayerController *AttackerController) override;

protected:
	// ��дHandleMatchHasStartedʹMatchStateת��ΪInProgressʱ(����Ϸ��ʼʱ)��ҽ��зֶ�
	virtual void HandleMatchHasStarted() override;

private:
	virtual void GameModeRestartGame() override;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "BlasterGameMode.generated.h"
 
// �Զ���MatchState
namespace MatchState
{
	extern BLASTER_API const FName Cooldown; // ��������,��ʾʤ�߲�����Cooldown��ʱ��
}
/**
 * �������ɶ�սʹ�õ�ģʽ
 */
UCLASS()
class BLASTER_API ABlasterGameMode : public AGameMode
{
	GENERATED_BODY()
public:
	ABlasterGameMode();
	virtual void Tick(float DeltaTime) override;

	virtual void PlayerEliminated(class ABlasterCharacter *ElimmedCharacter, 
		class ABlasterPlayerController *VictimController, ABlasterPlayerController *AttackerController);

	virtual void RequestRespawn(ACharacter *ElimmedCharacter, AController *ElimmedController);

	void PlayerLeftGame(class ABlasterPlayerState *PlayerLeaving);
	void PlayerLeftGameByController(class ABlasterPlayerController *PlayerLeaving);

	// ����˺�ǰ������Ϸģʽ����ʵ����ɵ��˺���,����������ֹ�Ѿ��˺�
	virtual float CalculateDamage(AController *Attacker, AController *Victim, float BaseDamage);

	// ����ʱ��
	UPROPERTY(EditDefaultsOnly)
	float WarmupTime = 10.f;

	// ����ʱ��
	UPROPERTY(EditDefaultsOnly)
	float MatchTime = 120.f;

	// Cooldownʱ��
	UPROPERTY(EditDefaultsOnly)
	float CooldownTime = 10.f;

	float LevelStartingTime = 0.f;

	bool bTeamsMatch = false;

protected:
	virtual void BeginPlay() override;
	virtual void OnMatchStateSet() override;

private:
	float CountdownTime = 0.f;

	class UBlasterGameInstance *BlasterGameInstance;
	
	virtual void GameModeRestartGame();
public:
	FORCEINLINE float GetCountdownTime() const { return CountdownTime; }
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "BlasterGameMode.generated.h"
 
namespace MatchState
{
	extern BLASTER_API const FName Cooldown; // 比赛结束,显示胜者并开启Cooldown计时器
}
/**
 * 
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
	virtual float CalculateDamage(AController *Attacker, AController *Victim, float BaseDamage);
	// 热身时长
	UPROPERTY(EditDefaultsOnly)
	float WarmupTime = 10.f;

	// 比赛时长
	UPROPERTY(EditDefaultsOnly)
	float MatchTime = 120.f;

	// Cooldown时长
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

	
public:
	FORCEINLINE float GetCountdownTime() const { return CountdownTime; }
};

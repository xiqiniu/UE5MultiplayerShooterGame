// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BlasterPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API ABlasterPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	void SetHUDHealth(float Health, float MaxHealth);
	void SetHUDScore(float Score);
	void SetHUDDefeats(int32 Defeats);
	void SetHUDWeaponAmmo(int32 Ammo);
	void SetHUDCarriedAmmo(int32 Ammo);
	void SetHUDMatchCountdown(float CountdownTime);
	void SetHUDAnnouncementCountdown(float CountdownTime);
	void SetHUDGrenades(int32 Grenades);
	virtual void OnPossess(APawn* InPawn) override;
	virtual float GetServerTime() const; 
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;

	virtual void ReceivedPlayer() override; // �ͻ��˼���󾡿��������ʱ��ͬ��
	void OnMatchStateSet(FName State);

	void HandleMatchHasStarted();
	void HandleCooldown();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	void SetHUDTime();
	void PollInit();

	/*
	* ͬ���������Ϳͻ��˵�ʱ��
	*/

	// �����������ǰʱ��,���Ϳͻ��˷��������ʱ��
	UFUNCTION(Server, Reliable)
	void ServerRequestServerTime(float TimeOfClientRequest);

	// �������ش�ͻ������յ������ʱ���Լ��ͻ��˷��������ʱ��
	UFUNCTION(Client, Reliable)
	void ClientReportServerTime(float TimeOfClientRequest, float TimeServerReceivedClientRequest);

	float ClientServerDelta = 0.f; // �ͻ������������ʱ���

	// ÿ��һ��ʱ��ͽ���һ��ͬ��
	UPROPERTY(EditAnywhere, Category = Time)
	float TimeSyncFrequency = 5.f; 

	float TimeSyncRunningTime = 0.f;
	void CheckTimeSync(float DeltaTime);

	// ��������Ҽ�����Ϸʱȷ�ϵ�ǰ��Ϸ״̬��RPC
	UFUNCTION(Server, Reliable)
	void ServerCheckMatchState();

	// ��������Ҽ�����Ϸʱ�ӷ�����������Ϸ��ǰ״̬��Client RPC
	UFUNCTION(Client, Reliable)
	void ClientJoinMidgame(FName StateOfMatch, float Warmup, float Match, float Cooldown, float StartingTime);

private:
	UPROPERTY()
	class ABlasterHUD *BlasterHUD;
	
	UPROPERTY()
	class ABlasterGameMode *BlasterGameMode;
	/*
	* ����Ϸ״̬��ص�ʱ��
	*/
	float LevelStartingTime = 0.f;
	float MatchTime = 0.f; 
	float WarmupTime = 0.f;
	float CooldownTime = 0.f;
	uint32 CountdownInt = 0;

	UPROPERTY(ReplicatedUsing = OnRep_MatchState)
	FName MatchState;

	UFUNCTION()
	void OnRep_MatchState();

	UPROPERTY()
	class UCharacterOverlay *CharacterOverlay;

	/*
	* ����ʧ��ʱ������Щֵ,��CharacterOverlay��Ч���������
	*/
	bool bInitializeCharacterOverlay = false;

	float HUDHealth;
	float HUDMaxHealth;
	float HUDScore;
	int32 HUDDefeats;
	int32 HUDGrenades;
};

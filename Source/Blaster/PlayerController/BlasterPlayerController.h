// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BlasterPlayerController.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHighPingDelegate, bool, bPingTooHigh);

/**
 * 
 */
UCLASS()
class BLASTER_API ABlasterPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	void SetHUDHealth(float Health, float MaxHealth);
	void SetHUDShield(float Shield, float MaxShield);
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

	float SingleTripTime = 0.f;
	FHighPingDelegate HighPingDelegate;
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

	// ��ʾ�ӳ�ͼ��
	void HighPingWarning();
	void StopHighPingWarning();
	void CheckPing(float DeltaTime);

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

	float HUDHealth;
	bool bInitializeHealth = false;
	float HUDMaxHealth;

	float HUDShield;
	float HUDMaxShield;
	bool bInitializeShield = false;

	float HUDScore;
	bool bInitializeScore = false;

	int32 HUDDefeats;
	bool bInitializeDefeats = false;

	int32 HUDGrenades;
	bool bInitializeGrenades  = false;

	float HUDCarriedAmmo;
	bool bInitializeCarriedAmmo = false;

	float HUDWeaponAmmo;
	bool bInitializeWeaponAmmo = false;

	/*
	* �ӳ����
	*/
	float HighPingRunningTime = 0.f;

	UPROPERTY(EditAnywhere)
	float HighPingDuration = 5.f;

	UPROPERTY(EditAnywhere)
	float CheckingPingFrequency = 20.f;

	UFUNCTION(Server, Reliable)
	void ServerReportPingStatus(bool bHighPing);

	UPROPERTY(EditAnywhere)
	float HighPingThreshold = 50.f;

	float PingAnimationRunningTime = 0.f;
};

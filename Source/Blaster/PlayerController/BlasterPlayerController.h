// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BlasterPlayerController.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHighPingDelegate, bool, bPingTooHigh);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLeftGameByController);
/**
 * 
 */
UCLASS()
class BLASTER_API ABlasterPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;
	/*
	* HUD���
	*/
	void SetHUDHealth(float Health, float MaxHealth);
	void SetHUDShield(float Shield, float MaxShield);
	void SetHUDScore(float Score);
	void SetHUDDefeats(int32 Defeats);
	void SetHUDWeaponAmmo(int32 Ammo);
	void SetHUDCarriedAmmo(int32 Ammo);
	void SetHUDMatchCountdown(float CountdownTime);
	void SetHUDAnnouncementCountdown(float CountdownTime);
	void SetHUDGrenades(int32 Grenades);
	void HideTeamScores();
	void InitTeamScores();
	void SetHUDRedTeamScores(int32 RedScore);
	void SetHUDBlueTeamScores(int32 BlueScore);

	// ���������,������ȥ���µĽ�ɫ�ᴥ��OnPossess,����������дOnpossess�����������Ѫ��UI�õ�����
	virtual void OnPossess(APawn* InPawn) override;

	void OnMatchStateSet(FName State);
	void HandleMatchHasStarted();
	void HandleCooldown();

	// ʱ��ͬ�����
	virtual float GetServerTime() const;
	float SingleTripTime = 0.f;
	virtual void ReceivedPlayer() override; // �ͻ��˼���󾡿��������ʱ��ͬ��
	void SetHUDTime();

	FHighPingDelegate HighPingDelegate;

	void BroadcastElim(APlayerState *Attacker, APlayerState *Victim);

	UFUNCTION(Server, Reliable)
	void ServerLeaveGame();

	FOnLeftGameByController OnLeftGameByController;

protected:
	virtual void SetupInputComponent() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
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
	void ClientJoinMidgame(FName StateOfMatch, float Warmup, float Match, float Cooldown, float StartingTime, bool bTeamMatch);

	UFUNCTION(Client, Reliable)
	void ClientElimAnnouncement(APlayerState *Attacker, APlayerState *Victim);

	// ��ȡ��Ϸ����ʱ��ʾ����Ϣ
	FText GetInfoText(const TArray<class ABlasterPlayerState *> &Players);
	FText GetTeamsInfoText(class ABlasterGameState *BlasterGameState);

private:
	// �Ƿ���ʾ�Ŷӷ���
	bool bShowTeamScores = false;

	UPROPERTY()
	class ABlasterHUD *BlasterHUD;
	
	/*
	* �ص����˵�
	*/
	void ShowReturnToMainMenu();

	UPROPERTY(EditAnywhere, Category = HUD)
	TSubclassOf<class UUserWidget> ReturnToMainMenuWidget;

	UPROPERTY()
	class UReturnToMainMenu *ReturnToMainMenu;

	bool bReturnToMainMenuOpen = false;

	UPROPERTY()
	class ABlasterGameMode *BlasterGameMode;

	UPROPERTY()
	class ABlasterPlayerState *BlasterPlayerState;

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
	* ����HUDʧ��ʱ������Щֵ,��CharacterOverlay��Ч���������
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

	bool bInitializeTeamScores = false;

	/*
	* �ӳ����
	*/
	// ��ʾ�ӳ�ͼ��
	void HighPingWarning();
	void StopHighPingWarning();
	void CheckPing(float DeltaTime);

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

	UFUNCTION(Client, Reliable)
	void ClientBroadcastLeftGameByController();

	/*UFUNCTION()
	void OnSkipWarmupChanged(bool bSkipWarmup);*/
};

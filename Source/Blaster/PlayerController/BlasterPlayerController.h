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
	* HUD相关
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

	// 玩家重生后,控制器去绑定新的角色会触发OnPossess,所以我们重写Onpossess让玩家重生后血量UI得到重置
	virtual void OnPossess(APawn* InPawn) override;

	void OnMatchStateSet(FName State);
	void HandleMatchHasStarted();
	void HandleCooldown();

	// 时钟同步相关
	virtual float GetServerTime() const;
	float SingleTripTime = 0.f;
	virtual void ReceivedPlayer() override; // 客户端加入后尽快与服务器时钟同步
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
	* 同步服务器和客户端的时间
	*/
	// 请求服务器当前时间,发送客户端发出请求的时间
	UFUNCTION(Server, Reliable)
	void ServerRequestServerTime(float TimeOfClientRequest);

	// 服务器回答客户端它收到请求的时间以及客户端发出请求的时间
	UFUNCTION(Client, Reliable)
	void ClientReportServerTime(float TimeOfClientRequest, float TimeServerReceivedClientRequest);

	float ClientServerDelta = 0.f; // 客户单与服务器的时间差

	// 每隔一段时间就进行一次同步
	UPROPERTY(EditAnywhere, Category = Time)
	float TimeSyncFrequency = 5.f; 

	float TimeSyncRunningTime = 0.f;

	void CheckTimeSync(float DeltaTime);

	// 用来在玩家加入游戏时确认当前游戏状态的RPC
	UFUNCTION(Server, Reliable)
	void ServerCheckMatchState();

	// 用来在玩家加入游戏时从服务器传递游戏当前状态的Client RPC
	UFUNCTION(Client, Reliable)
	void ClientJoinMidgame(FName StateOfMatch, float Warmup, float Match, float Cooldown, float StartingTime, bool bTeamMatch);

	UFUNCTION(Client, Reliable)
	void ClientElimAnnouncement(APlayerState *Attacker, APlayerState *Victim);

	// 获取游戏结束时显示的信息
	FText GetInfoText(const TArray<class ABlasterPlayerState *> &Players);
	FText GetTeamsInfoText(class ABlasterGameState *BlasterGameState);

private:
	// 是否显示团队分数
	bool bShowTeamScores = false;

	UPROPERTY()
	class ABlasterHUD *BlasterHUD;
	
	/*
	* 回到主菜单
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
	* 与游戏状态相关的时间
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
	* 设置HUD失败时存下这些值,在CharacterOverlay有效后进行设置
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
	* 延迟相关
	*/
	// 显示延迟图标
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

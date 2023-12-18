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

	virtual void ReceivedPlayer() override; // 客户端加入后尽快与服务器时钟同步
	void OnMatchStateSet(FName State);

	void HandleMatchHasStarted();
	void HandleCooldown();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	void SetHUDTime();
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
	void ClientJoinMidgame(FName StateOfMatch, float Warmup, float Match, float Cooldown, float StartingTime);

private:
	UPROPERTY()
	class ABlasterHUD *BlasterHUD;
	
	UPROPERTY()
	class ABlasterGameMode *BlasterGameMode;
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
	* 设置失败时存下这些值,在CharacterOverlay有效后进行设置
	*/
	bool bInitializeCharacterOverlay = false;

	float HUDHealth;
	float HUDMaxHealth;
	float HUDScore;
	int32 HUDDefeats;
	int32 HUDGrenades;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/TimelineComponent.h"
#include "Blaster/Interfaces/InteractWithCrosshairsInterface.h"
#include "Blaster/BlasterTypes/Team.h"
#include "Blaster/BlasterTypes/CombatState.h"
#include "Blaster/BlasterTypes/TurningInPlace.h"
#include "BlasterCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLeftGame);

UCLASS()
class BLASTER_API ABlasterCharacter : public ACharacter, public IInteractWithCrosshairsInterface
{
	GENERATED_BODY()

public:
	ABlasterCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent *PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;

	/*
	* 播放蒙太奇
	*/
	void PlayFireMontage(bool bAiming);
	void PlayElimMontage();
	void PlayThrowGrenadeMontage();
	void PlaySwapMontage();

	void PlayReloadMontage();

	UFUNCTION()
	void ReloadMontageEndedHandler(UAnimMontage *Montage, bool bInterrupted);

	UFUNCTION()
	void SwapMontageEndedHandler(UAnimMontage *Montage, bool bInterrupted);

	virtual void OnRep_ReplicatedMovement() override;

	// 加入下一轮游戏前禁止玩家行动
	UPROPERTY(Replicated)
	bool bDisableGameplay = false;

	// 玩家受击/死亡相关
	UFUNCTION()
	void ReceiveDamage(AActor *DamagedActor, float Damage, const UDamageType *DamageType,
		class AController *InstigatorController, AActor *DamageCauser);

	void PlayHitReactMontage();

	void Elim(bool bPlayerLeftGame);

	UFUNCTION(NetMultiCast, Reliable)
	void MulticastElim(bool bPlayerLeftGame);

	virtual void Destroyed() override;

	void DropOrDestroyWeapon(AWeapon *Weapon);
	void DropOrDestroyWeapons();

	/*
	* HUD相关
	*/
	UFUNCTION(BlueprintImplementableEvent)
	void ShowSniperScopeWidget(bool bShowScope);

	void UpdateHUDHealth();
	void UpdateHUDShield();
	void UpdateHUDAmmo();
	void SpawnDefaultWeapon();

	// 用于SSR的碰撞盒
	UPROPERTY()
	TMap<FName, class UBoxComponent *> HitCollisionBoxes;

	// 退出游戏相关
	UFUNCTION(Server, Reliable)
	void ServerLeaveGame();

	FOnLeftGame OnLeftGame;

	// 领先时显示光环
	UFUNCTION(NetMulticast, Reliable)
	void MulticastGainedTheLead();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastLostTheLead();

	void SetTeamColor(ETeam Team);

protected:
	virtual void BeginPlay() override;

	/*
	* 轴映射绑定
	*/
	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);

	/*
	* 操作映射绑定
	*/
	void RunButtonPressed();
	virtual void Jump() override;

	UFUNCTION(Server, Reliable)
	void ServerRunButtonPressed();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRunButtonPressed();

	void RunButtonReleased();

	UFUNCTION(Server, Reliable)
	void ServerRunButtonReleased();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRunButtonReleased();

	void EquipButtonPressed();

	UFUNCTION(Server, Reliable)
	void ServerEquipButtonPressed();

	void CrouchButtonPressed();
	void ReloadButtonPressed();
	void AimButtonPressed();
	void AimButtonReleased();
	void GrenadeButtonPressed();
	void FireButtonPressed();
	void FireButtonReleased();

	void SwapWeapon();

	UFUNCTION(Server, Reliable)
	void ServerSwapWeapon();

	// 瞄准和转向相关
	void AimOffset(float DeltaTime);
	void CalculateAO_Pitch();
	void SimProxiesTurn();
	void RotateInPlace(float DeltaTime);


	// 团队模式中设置出生点为所属团队的出生点
	void SetSpawnPoint();

	// 用于轮询初始化PlayerState时设置HUD,团队颜色,出生点
	void OnPlayerStateInitialized();

	// 初始化与HUD相关的类
	void PollInit();

	/*
	* 用于服务器倒带的碰撞盒
	*/
	UPROPERTY(EditAnywhere)
	class UBoxComponent *head;

	UPROPERTY(EditAnywhere)
	UBoxComponent *pelvis;

	UPROPERTY(EditAnywhere)
	UBoxComponent *spine_02;

	UPROPERTY(EditAnywhere)
	UBoxComponent *spine_03;

	UPROPERTY(EditAnywhere)
	UBoxComponent *upperarm_l;

	UPROPERTY(EditAnywhere)
	UBoxComponent *upperarm_r;

	UPROPERTY(EditAnywhere)
	UBoxComponent *lowerarm_l;

	UPROPERTY(EditAnywhere)
	UBoxComponent *lowerarm_r;

	UPROPERTY(EditAnywhere)
	UBoxComponent *hand_l;

	UPROPERTY(EditAnywhere)
	UBoxComponent *hand_r;

	UPROPERTY(EditAnywhere)
	UBoxComponent *backpack;

	UPROPERTY(EditAnywhere)
	UBoxComponent *blanket;

	UPROPERTY(EditAnywhere)
	UBoxComponent *thigh_l;

	UPROPERTY(EditAnywhere)
	UBoxComponent *thigh_r;

	UPROPERTY(EditAnywhere)
	UBoxComponent *calf_l;

	UPROPERTY(EditAnywhere)
	UBoxComponent *calf_r;

	UPROPERTY(EditAnywhere)
	UBoxComponent *foot_l;

	UPROPERTY(EditAnywhere)
	UBoxComponent *foot_r;

private:
	// 奔跑速度
	UPROPERTY(EditAnywhere)
	float RunSpeed = 800.f;
	
	float InitialMaxWalkSpeed = 600.f;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	class USpringArmComponent *CameraBoom;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	class UCameraComponent *FollowCamera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent *OverheadWidget;

	/*
	*  Blaster组件
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCombatComponent *Combat;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class ULagCompensationComponent *LagCompensation;

	UPROPERTY(VisibleAnywhere)
	class UBuffComponent *Buff;

	// 当前重叠的武器
	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	class AWeapon *OverlappingWeapon;

	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon *LastWeapon);

	// 玩家背靠墙壁时隐藏角色和武器防止遮挡摄像头
	void HideCameraIfCharacterClose();

	UPROPERTY(EditAnywhere)
	float CameraThreshold = 100.f;

	// 转向相关
	float AO_Yaw;
	float InterpAO_Yaw;
	float AO_Pitch;
	FRotator StartingAimRotation;
	ETurningInPlace TurningInPlace;
	void TurnInPlace(float DeltaTime);
	bool bRotateRootBone;
	FRotator ProxyRotationLastFrame;
	FRotator ProxyRotation;
	float ProxyYaw;
	float TimeSinceLastMovementReplication;
	float CalculateSpeed();

	// 设置simulated的角色播放旋转动画的阈值
	UPROPERTY(EditAnywhere)
	float TurnThreshold = 0.5f;

	/*
	*  动画蒙太奇
	*/
	UPROPERTY(EditAnywhere, Category = Combat)
	class UAnimMontage *FireWeaponMontage;
	
	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage *ReloadMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage *HitReactMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage *ElimMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage *ThrowGrenadeMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage *SwapMontage;

	/*
	* 血量
	*/
	UPROPERTY(ReplicatedUsing = OnRep_Health, EditAnywhere, Category = "Player Stats")
	float Health = 100.f;

	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float MaxHealth = 100.f;

	UFUNCTION()
	void OnRep_Health(float LastHealth);

	//护盾
	UPROPERTY(ReplicatedUsing = OnRep_Shield, EditAnywhere, Category = "Player Stats")
	float Shield = 0.f;

	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float MaxShield = 100.f;

	UFUNCTION()
	void OnRep_Shield(float LastShield);

	bool bElimmed = false;

	// 重生
	FTimerHandle ElimTimer;

	UPROPERTY(EditDefaultsOnly)
	float ElimDelay = 3.f;

	void ElimTimerFinished();

	bool bLeftGame = false;

	/*
	* 死亡时的溶解效果
	*/
	UPROPERTY(VisibleAnywhere)
	UTimelineComponent *DissolveTimeLine;

	FOnTimelineFloat DissolveTrack;

	UFUNCTION()
	void UpdateDissolveMaterial(float DissolveValue);

	void StartDissolve();

	UPROPERTY(EditAnywhere)
	UCurveFloat *DissolveCurve;

	// 运行时我们能动态设置的MI
	UPROPERTY(VisibleAnywhere, Category = Elim) 
	UMaterialInstanceDynamic *DynamicDissolveMaterialInstance;

	// 蓝图中设置的MI
	UPROPERTY(VisibleAnywhere, Category = Elim)
	UMaterialInstance *DissolveMaterialInstance;

	/*
	* 团队颜色和死亡溶解效果
	*/
	UPROPERTY(EditAnywhere, Category = Elim)
	UMaterialInstance *RedDissolveMatInst;	
	
	UPROPERTY(EditAnywhere, Category = Elim)
	UMaterialInstance *RedMaterial;

	UPROPERTY(EditAnywhere, Category = Elim)
	UMaterialInstance *BlueDissolveMatInst;

	UPROPERTY(EditAnywhere, Category = Elim)
	UMaterialInstance *BlueMaterial;

	UPROPERTY(EditAnywhere, Category = Elim)
	UMaterialInstance *OriginalMaterial;

	UPROPERTY(EditAnywhere, Category = Elim)
	UMaterialInstance *OriginalDissolveMatInst;

	/*
	* 死亡时头上的机器人
	*/
	UPROPERTY(EditAnywhere, Category = Elim)
	UParticleSystem *ElimBotEffect;

	UPROPERTY(VisibleAnywhere, Category = Elim)
	UParticleSystemComponent *ElimBotComponent;

	UPROPERTY(EditAnywhere, Category = Elim)
	class USoundCue *ElimBotSound;

	UPROPERTY()
	class ABlasterPlayerController *BlasterPlayerController;

	UPROPERTY()
	class ABlasterPlayerState *BlasterPlayerState;

	// 领先时显示的光环
	UPROPERTY(EditAnywhere)
	class UNiagaraSystem *CrownSystem;

	UPROPERTY()
	class UNiagaraComponent *CrownComponent;

	/*
	* 手雷
	*/
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent *AttachedGrenade;

	/*
	* 默认武器
	*/
	UPROPERTY(EditAnywhere)
	TSubclassOf<AWeapon> DefaultWeaponClass;

	UPROPERTY()
	class ABlasterGameMode *BlasterGameMode;

public:	
	void SetOverlappingWeapon(AWeapon *Weapon);

	FORCEINLINE float GetAO_Yaw() const { return AO_Yaw; }
	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; }
	FORCEINLINE ETurningInPlace GetTurningInPlace() const { return TurningInPlace; }
	FORCEINLINE UCameraComponent *GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE bool ShouldRotateRootBone() const { return bRotateRootBone; }

	FORCEINLINE void SetHealth(float Amount) { Health = Amount;}
	FORCEINLINE float GetHealth() const { return Health; }
	FORCEINLINE void SetShield(float Amount) { Shield = Amount; }
	FORCEINLINE float GetShield() const { return Shield; }
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }
	FORCEINLINE float GetMaxShield() const { return MaxShield; }

	AWeapon *GetEquippedWeapon();
	FVector GetHitTarget() const;
	ECombatState GetCombatState() const;

	FORCEINLINE UCombatComponent *GetCombat() const { return Combat; }
	FORCEINLINE UBuffComponent *GetBuff() const { return Buff; }
	FORCEINLINE ULagCompensationComponent *GetLagCompensation() const { return LagCompensation; }


	FORCEINLINE bool GetDisableGameplay() const { return bDisableGameplay; }
	FORCEINLINE UAnimMontage *GetReloadMontage() const { return ReloadMontage; }
	FORCEINLINE UStaticMeshComponent *GetAttachedGrenade() const { return AttachedGrenade; }

	FORCEINLINE bool IsElimmed() const { return bElimmed; }
	bool IsWeaponEquipped();
	bool IsAiming();
	bool IsLocallyReloading() const;
	bool IsLocallySwapping() const;
	bool IsHoldingTheFlag() const;

	ETeam GetTeam();
	void SetHoldingTheFlag(bool bHolding);
};

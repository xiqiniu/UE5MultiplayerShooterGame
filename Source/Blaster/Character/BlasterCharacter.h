// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Blaster/BlasterTypes/TurningInPlace.h"
#include "Blaster/Interfaces/InteractWithCrosshairsInterface.h"
#include "Components/TimelineComponent.h"
#include "Blaster/BlasterTypes/CombatState.h"
#include "BlasterCharacter.generated.h"

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
	void PlayFireMontage(bool bAiming);
	void PlayReloadMontage();
	void PlayElimMontage();
	void PlayThrowGrenadeMontage();
	virtual void OnRep_ReplicatedMovement() override;

	UPROPERTY(Replicated)
	bool bDisableGameplay = false;

	// 玩家死亡
	void Elim();

	UFUNCTION(NetMultiCast, Reliable)
	void MulticastElim();

	virtual void Destroyed() override;

	UFUNCTION(BlueprintImplementableEvent)
	void ShowSniperScopeWidget(bool bShowScope);

	void UpdateHUDHealth();
	void UpdateHUDShield();
	void UpdateHUDAmmo();
	void SpawnDefaultWeapon();

protected:
	virtual void BeginPlay() override;
	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void EquipButtonPressed();
	void CrouchButtonPressed();
	void ReloadButtonPressed();
	void AimButtonPressed();
	void AimButtonReleased();
	void GrenadeButtonPressed();
	void AimOffset(float DeltaTime);
	void CalculateAO_Pitch();
	void SimProxiesTurn();
	virtual void Jump() override;
	void FireButtonPressed();
	void FireButtonReleased();
	void PlayHitReactMontage();
	void DropOrDestroyWeapon(AWeapon *Weapon);
	void DropOrDestroyWeapons();
	void SwapWeapon();

	UFUNCTION(Server, Reliable)
	void ServerSwapWeapon();

	UFUNCTION()
	void ReceiveDamage(AActor *DamagedActor, float Damage, const UDamageType *DamageType, 
		class AController * InstigatorController,AActor *DamageCauser);

	// 初始化与HUD相关的类
	void PollInit();
	void RotateInPlace(float DeltaTime);

private:
	UPROPERTY(VisibleAnywhere, Category = Camera)
	class USpringArmComponent *CameraBoom;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	class UCameraComponent *FollowCamera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent *OverheadWidget;

	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	class AWeapon *OverlappingWeapon;

	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon *LastWeapon);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCombatComponent *Combat;

	UPROPERTY(VisibleAnywhere)
	class UBuffComponent *Buff;

	UFUNCTION(Server, Reliable)
	void ServerEquipButtonPressed();

	void HideCameraIfCharacterClose();

	UPROPERTY(EditAnywhere)
	float CameraThreshold = 200.f;

	float AO_Yaw;
	float InterpAO_Yaw;
	float AO_Pitch;
	FRotator StartingAimRotation;
	ETurningInPlace TurningInPlace;
	void TurnInPlace(float DeltaTime);

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

	bool bRotateRootBone;
	// 设置simulated的角色播放旋转动画的阈值
	UPROPERTY(EditAnywhere)
	float TurnThreshold = 0.5f;

	FRotator ProxyRotationLastFrame;
	FRotator ProxyRotation;
	float ProxyYaw;
	float TimeSinceLastMovementReplication;
	float CalculateSpeed();

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

	/*
	* 溶解效果
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
	UPROPERTY(EditAnywhere, Category = Elim)
	UMaterialInstance *DissolveMaterialInstance;

	/*
	* Elim Bot
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

public:	
	void SetOverlappingWeapon(AWeapon *Weapon);
	bool IsWeaponEquipped();
	bool IsAiming();
	FORCEINLINE float GetAO_Yaw() const { return AO_Yaw; }
	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; }
	AWeapon *GetEquippedWeapon(); 
	FORCEINLINE ETurningInPlace GetTurningInPlace() const { return TurningInPlace; }
	FVector GetHitTarget() const;
	FORCEINLINE UCameraComponent *GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE bool ShouldRotateRootBone() const { return bRotateRootBone; }
	FORCEINLINE bool IsElimmed() const { return bElimmed; }
	FORCEINLINE void SetHealth(float Amount) { Health = Amount;}
	FORCEINLINE float GetHealth() const { return Health; }
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }
	FORCEINLINE void SetShield(float Amount) { Shield = Amount; }
	FORCEINLINE float GetShield() const { return Shield; }
	FORCEINLINE float GetMaxShield() const { return MaxShield; }
	ECombatState GetCombatState() const;
	FORCEINLINE UCombatComponent *GetCombat() const { return Combat; }
	FORCEINLINE UBuffComponent *GetBuff() const { return Buff; }
	FORCEINLINE bool GetDisableGameplay() const { return bDisableGameplay; }
	FORCEINLINE UAnimMontage *GetReloadMontage() const { return ReloadMontage; }
	FORCEINLINE UStaticMeshComponent *GetAttachedGrenade() const { return AttachedGrenade; }
};

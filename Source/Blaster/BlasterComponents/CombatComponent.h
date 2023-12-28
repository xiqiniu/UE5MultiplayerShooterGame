// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include"Blaster/HUD/BlasterHUD.h" 
#include"Blaster/Weapon/WeaponTypes.h"
#include"Blaster/BlasterTypes/CombatState.h"
#include "CombatComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLASTER_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()
	friend class ABlasterCharacter;
public:	
	UCombatComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;

	void EquipWeapon(class AWeapon *WeaponToEquip);
	void SwapWeapons();
	void Reload();

	// ���ö���֪ͨ����,����Ҫд��BlueprintCallable
	UFUNCTION(BlueprintCallable)
	void FinishReloading();

	UFUNCTION(BlueprintCallable)
	void FinishSwap();

	UFUNCTION(BlueprintCallable)
	void FinishSwapAttachWeapon();

	UFUNCTION(BlueprintCallable)
	void ShotgunShellReload();

	void FireButtonPressed(bool bPressed);

	void JumpToShotgunEnd();

	UFUNCTION(BlueprintCallable)
	void ThrowGrenadeFinished();

	UFUNCTION(BlueprintCallable) 
	void LaunchGrenade();
	 
	UFUNCTION(Server, Reliable)
	void ServerLaunchGrenade(const FVector_NetQuantize &Target);

	void PickupAmmo(EWeaponType WeaponType, int32 AmmoAmount);

	// ���ڴ����ز���װ������ʱ����fabric�ƶ�����
	bool bLocallyReloading = false;

	bool bLocallySwapping = false;

protected:
	virtual void BeginPlay() override;

	/*
	* �������
	*/
	void Fire();
	void FireProjectileWeapon();
	void FireHitScanWeapon();
	void FireShotgun();

	void LocalFire(const FVector_NetQuantize &TraceHitTarget);
	void ShotgunLocalFire(const TArray<FVector_NetQuantize> &TraceHitTargets);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerFire(const FVector_NetQuantize &TraceHitTarget, float FireDelay);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastFire(const FVector_NetQuantize &TraceHitTarget);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerShotgunFire(const TArray<FVector_NetQuantize> &TraceHitTargets, float FireDelay);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastShotgunFire(const TArray<FVector_NetQuantize> &TraceHitTargets);

	/*
	* �������
	*/
	void ThrowGrenade();

	UFUNCTION(Server, Reliable)
	void ServerThrowGrenade();

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AProjectile> GrenadeClass;

	// ��׼���
	void TraceUnderCrosshairs(FHitResult &TraceHitResult);

	void SetHUDCrosshairs(float DeltaTime);

	void SetAiming(bool bIsAiming);

	UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool bIsAiming);

	// װ�����
	UFUNCTION(Server, Reliable)
	void ServerReload();

	void HandleReload();

	int32 AmountToReload();

	void ReloadEmptyWeapon();

	void AttachActorToRightHand(AActor *ActorToAttach);
	void AttachActorToLeftHand(AActor *ActorToAttach);
	void AttachFlagToLeftHand(AWeapon *Flag);
	void AttachActorToBackpack(AActor *ActorToAttach);

	void UpdateCarriedAmmo();
	void ShowAttachedGrenade(bool bShowGrenade);

	void EquipPrimaryWeapon(AWeapon *WeaponToEquip);
	void DropEquippedWeapon(); // ����EquipPrimaryWeaponʱ,�������������Ҫ�ȶ���
	void EquipSecondaryWeapon(AWeapon *WeaponToEquip);
	void PlayEquippedWeaponSound(AWeapon *WeaponToEquip);

private:
	UPROPERTY()
	class ABlasterCharacter *Character;

	UPROPERTY()
	class ABlasterPlayerController *Controller;

	UPROPERTY()
	class ABlasterHUD *HUD;

	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	class AWeapon *EquippedWeapon;

	UPROPERTY(ReplicatedUsing = OnRep_SecondaryWeapon)
	AWeapon *SecondaryWeapon;

	UFUNCTION()
	void OnRep_EquippedWeapon();

	UFUNCTION()
	void OnRep_SecondaryWeapon();

	/*
	* ׼�����
	*/
	float CrosshairVelocityFactor;
	float CrosshairInAirFactor;
	float CrosshairAimFactor;
	float CrosshairShootingFactor;
	FHUDPackage HUDPackage;
	FVector HitTarget;

	/*
	* ��׼���
	*/
	UPROPERTY(ReplicatedUsing = OnRep_Aiming)
	bool bAiming = false;

	bool bAimButtonPressed = false;

	UFUNCTION()
	void OnRep_Aiming();

	UPROPERTY(EditAnywhere)
	float BaseWalkSpeed;

	UPROPERTY(EditAnywhere)
	float AimWalkSpeed;

	// ����׼ʱ���FOV, ��BeginPlay����
	float DefaultFOV;

	UPROPERTY(EditAnywhere, Category = Combat)
	float ZoomedFOV = 30.f;

	float CurrentFOV;

	UPROPERTY(EditAnywhere, Category = Combat)
	float ZoomInterpSpeed = 20.f;

	void InterpFOV(float DeltaTime);

	/*
	* �������
	*/
	bool bFireButtonPressed;

	FTimerHandle FireTimer;

	bool bCanFire = true;
	void StartFireTimer();
	void FireTimerFinished();

	// �ж��ܷ񿪻�
	bool CanFire();

	/*
	* ��ҩ���
	*/
	// ��ɫ��ǰЯ����������ĵ�ҩ��,�����������������TMap
	// ��Ϊ�������Ϳͻ����ù�ϣ������Ľ����һ��һ��, TMap���޷����Ƶ�
	UPROPERTY(ReplicatedUsing = OnRep_CarriedAmmo)
	int32 CarriedAmmo;

	UFUNCTION()
	void OnRep_CarriedAmmo();

	TMap<EWeaponType, int32> CarriedAmmoMap;

	UPROPERTY(EditAnywhere)
	int32 MaxCarriedAmmo = 500;

	UPROPERTY(EditAnywhere)
	int32 StartingARAmmo = 30;

	UPROPERTY(EditAnywhere)
	int32 StartingRocketAmmo = 0;	
	
	UPROPERTY(EditAnywhere)
	int32 StartingPistolAmmo = 0;
	
	UPROPERTY(EditAnywhere)
	int32 StartingSMGAmmo = 0;
	
	UPROPERTY(EditAnywhere)
	int32 StartingShotgunAmmo = 0;	
	
	UPROPERTY(EditAnywhere)
	int32 StartingSniperRifleAmmo = 0;
	
	UPROPERTY(EditAnywhere)
	int32 StartingGrenadeLauncherAmmo = 0;
	
	UPROPERTY(ReplicatedUsing = OnRep_Grenades)
	int32 Grenades = 4;

	UFUNCTION()
	void OnRep_Grenades();

	UPROPERTY(EditAnywhere)
	int32 MaxGrenades = 4;

	void InitializeCarriedAmmo();
	void UpdateHUDGrenades();
	void UpdateAmmoValues();
	void UpdateShotgunAmmoValues();

	UPROPERTY(ReplicatedUsing = OnRep_CombatState)
	ECombatState CombatState = ECombatState::ECS_Unoccupied;

	UFUNCTION()
	void OnRep_CombatState();

	UPROPERTY(ReplicatedUsing = OnRep_HoldingTheFlag)
	bool bHoldingTheFlag = false;

	UFUNCTION()
	void OnRep_HoldingTheFlag();

	UPROPERTY()
	AWeapon *TheFlag;

public:	
	FORCEINLINE int32 GetGrenades() const { return Grenades; }
	bool ShouldSwapWeapons();
};

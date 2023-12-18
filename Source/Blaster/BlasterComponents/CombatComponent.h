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
	void Reload();

	// 利用动画通知调用,所以要写成BlueprintCallable
	UFUNCTION(BlueprintCallable)
	void FinishReloading();
	void FireButtonPressed(bool bPressed);

	UFUNCTION(BlueprintCallable)
	void ShotgunShellReload();

	void JumpToShotgunEnd();

	UFUNCTION(BlueprintCallable)
	void ThrowGrenadeFinished();

	UFUNCTION(BlueprintCallable) 
	void LaunchGrenade();
	 
	UFUNCTION(Server, Reliable)
	void ServerLaunchGrenade(const FVector_NetQuantize &Target);
protected:
	virtual void BeginPlay() override;

	void SetAiming(bool bIsAiming);
	
	UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool bIsAiming);

	UFUNCTION()
	void OnRep_EquippedWeapon();

	void Fire();

	UFUNCTION(Server, Reliable)
	void ServerFire(const FVector_NetQuantize &TraceHitResult);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastFire(const FVector_NetQuantize &TraceHitResult);

	void ThrowGrenade();

	UFUNCTION(Server, Reliable)
	void ServerThrowGrenade();

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AProjectile> GrenadeClass;

	void TraceUnderCrosshairs(FHitResult &TraceHitResult);

	void SetHUDCrosshairs(float DeltaTime);

	UFUNCTION(Server, Reliable)
	void ServerReload();

	void HandleReload();

	int32 AmountToReload();

	void DropEquippedWeapon();
	void AttachActorToRightHand(AActor *ActorToAttach);
	void AttachActorToLeftHand(AActor *ActorToAttach);
	void UpdateCarriedAmmo();
	void PlayEquippedWeaponSound();
	void ReloadEmptyWeapon();
	void ShowAttachedGrenade(bool bShowGrenade);
private:
	UPROPERTY()
	class ABlasterCharacter *Character;

	UPROPERTY()
	class ABlasterPlayerController *Controller;

	UPROPERTY()
	class ABlasterHUD *HUD;

	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	class AWeapon *EquippedWeapon;

	UPROPERTY(Replicated)
	bool bAiming;

	UPROPERTY(EditAnywhere)
	float BaseWalkSpeed;

	UPROPERTY(EditAnywhere)
	float AimWalkSpeed;

	bool bFireButtonPressed;

	/*
	* HUD and crosshairs
	*/
	float CrosshairVelocityFactor;
	float CrosshairInAirFactor;
	float CrosshairAimFactor;
	float CrosshairShootingFactor;
	FHUDPackage HUDPackage;
	FVector HitTarget;

	/*
	* Aiming and FOV
	*/
	// 不瞄准时候的FOV, 在BeginPlay设置
	float DefaultFOV;

	UPROPERTY(EditAnywhere, Category = Combat)
	float ZoomedFOV = 30.f;

	float CurrentFOV;
	UPROPERTY(EditAnywhere, Category = Combat)
	float ZoomInterpSpeed = 20.f;

	void InterpFOV(float DeltaTime);

	/*
	* Automatic Fire
	*/
	FTimerHandle FireTimer;

	bool bCanFire = true;
	void StartFireTimer();
	void FireTimerFinished();

	// 判断能否开火
	bool CanFire();

	// 角色当前携带武器种类的弹药量,复制这个而不是整个TMap,因为服务器和客户端用哈希算出来的结果不一定一样, TMap是无法复制的
	UPROPERTY(ReplicatedUsing = OnRep_CarriedAmmo)
	int32 CarriedAmmo;

	UFUNCTION()
	void OnRep_CarriedAmmo();

	TMap<EWeaponType, int32> CarriedAmmoMap;

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

	void InitializeCarriedAmmo();

	UPROPERTY(ReplicatedUsing = OnRep_CombatState)
	ECombatState CombatState = ECombatState::ECS_Unoccupied;

	UFUNCTION()
	void OnRep_CombatState();

	void UpdateAmmoValues();
	void UpdateShotgunAmmoValues();
	
	UPROPERTY(ReplicatedUsing = OnRep_Grenades)
	int32 Grenades = 4;

	UFUNCTION()
	void OnRep_Grenades();

	UPROPERTY(EditAnywhere)
	int32 MaxGrenades = 4;

	void UpdateHUDGrenades();
public:	
	FORCEINLINE int32 GetGrenades() const { return Grenades; }
};

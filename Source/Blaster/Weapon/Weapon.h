// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponTypes.h"
#include "Blaster/BlasterTypes/Team.h"
#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EFireType : uint8 
{
	EFT_HitScan UMETA(DiaplayName = "Hit Scan Weapon"),
	EFT_Projectle UMETA(DiaplayName = "Projectile Weapon"),
	EFT_Shotgun UMETA(DiaplayName = "Shotgun Weapon"),
	EFT_Max UMETA(DiaplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class EWeaponState : uint8 
{
	EWS_Initial UMETA(DisplayName = "Initial State"),
	EWS_Euipped UMETA(DisplayName = "Equipped"),
	EWS_EuippedSecondary UMETA(DisplayName = "Equipped Secondary"),
	EWS_Dropped UMETA(DisplayName = "Dropped"),
	// 用于确认这个枚举里有多少种
	EWS_MAX UMETA(DisplayName = "DefaultMAX")
};

UCLASS()
class BLASTER_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeapon();
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;
	virtual void OnRep_Owner() override;
	void ShowPickupWidget(bool bShowWidget);
	virtual void Fire(const FVector &HitTarget);
	virtual void Dropped();
	void AddAmmo(int32 AmmoToAdd);
	void SetHUDAmmo();

	/*
	* 准星使用的纹理(不同武器的准星不同)
	*/
	UPROPERTY(EditAnywhere, Category = Crosshairs)
	class UTexture2D *CrosshairsCenter;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
	class UTexture2D *CrosshairsLeft;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
	class UTexture2D *CrosshairsRight;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
	class UTexture2D *CrosshairsTop;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
	class UTexture2D *CrosshairsBottom;

	/*
	* 瞄准时减小FOV
	*/
	UPROPERTY(EditAnywhere)
	float ZoomedFOV = 30.f;

	UPROPERTY(EditAnywhere)
	float ZoomInterpSpeed = 20.f;

	/*
	* 自动开火
	*/
	UPROPERTY(EditAnywhere, Category = Combat)
	float FireDelay = 0.15f;

	UPROPERTY(EditAnywhere, Category = Combat)
	bool bAutomatic = true;

	// 启用或禁用自定义深度
	void EnableCustomDepth(bool bEnable);

	// 销毁玩家出生自带的武器
	bool bDestroyWeapon = false;

	UPROPERTY(EditAnywhere)
	EFireType FireType;

	UPROPERTY(EditAnywhere, Category = "Weapon Scatter")
	bool bUseScatter = false;

	UPROPERTY(EditAnywhere)
	class USoundCue *EquipSound;

	FVector TraceEndWithScatter(const FVector &HitTarget);

protected:
	virtual void BeginPlay() override;	

	virtual void OnWeaponStateSet();
	virtual void OnEquipped();
	virtual void OnDropped();
	virtual void OnEquippedSecondary();

	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, 
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

	UFUNCTION()
	void OnSphereEndOverlap(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp,
		int32 OtherBodyIndex);
	
	/*
	* 散射
	*/
	UPROPERTY(EditAnywhere, Category = "Weapon Scatter")
	float DistanceToSphere = 800.f;

	UPROPERTY(EditAnywhere, Category = "Weapon Scatter")
	float SphereRadius = 75.f;

	// 伤害
	UPROPERTY(EditAnywhere)
	float Damage = 20.f;

	UPROPERTY(EditAnywhere)
	float HeadShotDamage = 40.f;

	UPROPERTY(Replicated, EditAnywhere)
	bool bUseServerSideRewind = false;

	UPROPERTY()
	class ABlasterCharacter *BlasterOwnerCharacter;

	UPROPERTY()
	class ABlasterPlayerController *BlasterOwnerController;

	UFUNCTION()
	void OnPingTooHigh(bool bPingTooHigh);

private:
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	USkeletalMeshComponent *WeaponMesh;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	class USphereComponent *AreaSphere;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	class UWidgetComponent *PickUpWidget;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	class UAnimationAsset *FireAnimation;

	// 弹壳
	UPROPERTY(EditAnywhere)
	TSubclassOf<class ACasing> CasingClass;

	/*
	* 子弹
	*/
	UPROPERTY(EditAnywhere)
	int32 Ammo;
	
	UPROPERTY(EditAnywhere)
	int32 MagCapacity;

	UPROPERTY(ReplicatedUsing = OnRep_WeaponSate, VisibleAnywhere, Category = "Weapon Properties")
	EWeaponState WeaponState;

	UFUNCTION()
	void OnRep_WeaponSate();

	// 未处理的服务器请求数量
	// 在SpendRound中增加, 在ClientUpdateAmmo中减少
	int32 Sequence = 0;

	// 消耗一个子弹
	void SpendRound();

	UFUNCTION(Client, Reliable)
	void ClientUpdateAmmo(int32 ServerAmmo);

	UFUNCTION(Client, Reliable)
	void ClientAddAmmo(int32 AmmoToAdd);

	UPROPERTY(EditAnywhere)
	EWeaponType WeaponType;

	UPROPERTY(EditAnywhere)
	ETeam Team;

public:	
	FORCEINLINE USphereComponent *GetAreaSphere() const { return AreaSphere; }
	FORCEINLINE USkeletalMeshComponent *GetWeaponMesh() const { return WeaponMesh; }
	FORCEINLINE UWidgetComponent *GetPickupWidget() const { return PickUpWidget; }
	FORCEINLINE float GetZoomedFOV() const { return ZoomedFOV; }
	FORCEINLINE float GetZoomedInterpSpeed() const { return ZoomInterpSpeed; }
	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }
	FORCEINLINE int32 GetAmmo() const { return Ammo; }
	FORCEINLINE int32 GetMagCapacity() const { return MagCapacity; }
	FORCEINLINE float GetDamage() const { return Damage; }
	FORCEINLINE float GetHeadShotDamage() const { return HeadShotDamage; }
	FORCEINLINE ETeam GetTeam() const { return Team; }

	void SetWeaponState(EWeaponState State);
	bool IsEmpty() const; 
	bool IsFull() const; 
};

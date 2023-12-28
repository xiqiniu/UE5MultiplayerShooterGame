// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LagCompensationComponent.generated.h"

// 用于SSR的碰撞盒的信息
USTRUCT(BlueprintType)
struct FBoxInformation
{
	GENERATED_BODY()

	UPROPERTY()
	FVector Location;

	UPROPERTY()
	FRotator Rotation;

	UPROPERTY()
	FVector BoxExtent;
};

// 启用SSR时,每一帧要存储的信息
USTRUCT(BlueprintType)
struct FFramePackage
{
	GENERATED_BODY()
	
	UPROPERTY()
	float Time;

	UPROPERTY()
	TMap<FName, FBoxInformation> HitBoxInfo;

	UPROPERTY()
	ABlasterCharacter *Character;
};

// 普通武器的SSR结果,只需返回是否打中和是否爆头
USTRUCT(BlueprintType)
struct FServerSideRewindResult
{
	GENERATED_BODY()

	UPROPERTY()
	bool bHitConfirmed;

	UPROPERTY()
	bool bHeadShot;
};

// 霰弹的SSR结果,需要返回两个Map,键是打中的玩家,值是打中了几次
USTRUCT(BlueprintType)
struct FShotgunServerSideRewindResult
{
	GENERATED_BODY()

	UPROPERTY()
	TMap<ABlasterCharacter *, uint32> HeadShots;

	UPROPERTY()
	TMap<ABlasterCharacter *, uint32> BodyShots;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLASTER_API ULagCompensationComponent : public UActorComponent
{
	GENERATED_BODY()

	friend class ABlasterCharacter;
public:	
	ULagCompensationComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	void ShowFramePackage(const FFramePackage &Package, const FColor Color);

	// 扫描类
	FServerSideRewindResult ServerSideRewind(
		class ABlasterCharacter *HitCharacter, 
		const FVector_NetQuantize &TraceStart,  
		const FVector_NetQuantize HitLocation, 
		float HitTime
	);

	// 弹药类
	FServerSideRewindResult ProjectileServerSideRewind(
		ABlasterCharacter *HitCharacter,
		const FVector_NetQuantize &TraceStart,
		const FVector_NetQuantize100 InitialVelocity,
		float HitTime
	);

	// 霰弹
	FShotgunServerSideRewindResult ShotgunServerSideRewind(
		const TArray<ABlasterCharacter *> &HitCharacters,
		const FVector_NetQuantize &TraceStart,
		const TArray<FVector_NetQuantize> &HitLocations,
		float HitTime
	);

	UFUNCTION(Server, Reliable)
	void ServerScoreRequest(
		ABlasterCharacter *HitCharacter,
		const FVector_NetQuantize &TraceStart,
		const FVector_NetQuantize &HitLocation,
		float HitTime
	);

	UFUNCTION(Server, Reliable)
	void ProjectileServerScoreRequest(
		ABlasterCharacter *HitCharacter,
		const FVector_NetQuantize &TraceStart,
		const FVector_NetQuantize100 InitialVelocity,
		float HitTime
	);

	UFUNCTION(Server, Reliable)
	void ShotgunServerScoreRequest(
		const TArray<ABlasterCharacter *> &HitCharacters,
		const FVector_NetQuantize &TraceStart,
		const TArray<FVector_NetQuantize> &HitLocations,
		float HitTime
	);

protected:
	virtual void BeginPlay() override;
	// 记录帧信息
	void SaveFramePackage(FFramePackage &Package);
	void SaveFramePackage();

	// 存储当前角色位置的碰撞盒,方便做完SSR检测后放回去
	void CacheBoxPositions(ABlasterCharacter *HitCharacter, FFramePackage &OutFramePackage);

	// 根据帧信息移动碰撞盒
	void MoveBoxes(ABlasterCharacter *HitCharacter, const FFramePackage& Package);

	// 还原碰撞盒的位置
	void ResetHitBoxes(ABlasterCharacter *HitCharacter, const FFramePackage &Package);

	// 做SSR检测时需要禁用角色的Mesh,我们只关注是否打中了那一帧的碰撞盒
	void EnableCharacterMeshCollision(ABlasterCharacter *HitCharacter, ECollisionEnabled::Type CollisionEnabled);

	// 找到距离HitTime最近的那一帧(利用了插值)
	FFramePackage GetFrameToCheck(ABlasterCharacter *HitCharacter, float HitTime);

	// 用离HitTime最接近的两帧的信息进行插值
	FFramePackage InterpBetweenFrames(const FFramePackage &OlderFrame, const FFramePackage &YoungerFrame, const float HitTime);

	// 扫描类SSR检测是否击中
	FServerSideRewindResult ConfirmHit(
		const FFramePackage &Package,
		ABlasterCharacter *HitCharacter,
		const FVector_NetQuantize &TraceStart,
		const FVector_NetQuantize &HitLocation
	);

	// 弹药类SSR检测是否击中
	FServerSideRewindResult ProjectileConfirmHit(
		const FFramePackage &Package,
		ABlasterCharacter *HitCharacter,
		const FVector_NetQuantize &TraceStart,
		const FVector_NetQuantize100 InitialVelocity,
		float HitTime
	);

	// 霰弹SSR检测是否击中
	FShotgunServerSideRewindResult ShotgunConfirmHit(
		const TArray<FFramePackage> &FramePackages,
		const FVector_NetQuantize &TraceStart,
		const TArray<FVector_NetQuantize> &HitLocations
		);

private:
	UPROPERTY()
	ABlasterCharacter *Character;

	UPROPERTY()
	class ABlasterPlayerController *Controller;

	TDoubleLinkedList<FFramePackage> FrameHistory;

	UPROPERTY(EditAnywhere)
	float MaxRecordTime = 4.f;
};

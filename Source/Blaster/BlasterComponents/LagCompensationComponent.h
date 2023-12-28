// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LagCompensationComponent.generated.h"

// ����SSR����ײ�е���Ϣ
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

// ����SSRʱ,ÿһ֡Ҫ�洢����Ϣ
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

// ��ͨ������SSR���,ֻ�践���Ƿ���к��Ƿ�ͷ
USTRUCT(BlueprintType)
struct FServerSideRewindResult
{
	GENERATED_BODY()

	UPROPERTY()
	bool bHitConfirmed;

	UPROPERTY()
	bool bHeadShot;
};

// ������SSR���,��Ҫ��������Map,���Ǵ��е����,ֵ�Ǵ����˼���
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

	// ɨ����
	FServerSideRewindResult ServerSideRewind(
		class ABlasterCharacter *HitCharacter, 
		const FVector_NetQuantize &TraceStart,  
		const FVector_NetQuantize HitLocation, 
		float HitTime
	);

	// ��ҩ��
	FServerSideRewindResult ProjectileServerSideRewind(
		ABlasterCharacter *HitCharacter,
		const FVector_NetQuantize &TraceStart,
		const FVector_NetQuantize100 InitialVelocity,
		float HitTime
	);

	// ����
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
	// ��¼֡��Ϣ
	void SaveFramePackage(FFramePackage &Package);
	void SaveFramePackage();

	// �洢��ǰ��ɫλ�õ���ײ��,��������SSR����Ż�ȥ
	void CacheBoxPositions(ABlasterCharacter *HitCharacter, FFramePackage &OutFramePackage);

	// ����֡��Ϣ�ƶ���ײ��
	void MoveBoxes(ABlasterCharacter *HitCharacter, const FFramePackage& Package);

	// ��ԭ��ײ�е�λ��
	void ResetHitBoxes(ABlasterCharacter *HitCharacter, const FFramePackage &Package);

	// ��SSR���ʱ��Ҫ���ý�ɫ��Mesh,����ֻ��ע�Ƿ��������һ֡����ײ��
	void EnableCharacterMeshCollision(ABlasterCharacter *HitCharacter, ECollisionEnabled::Type CollisionEnabled);

	// �ҵ�����HitTime�������һ֡(�����˲�ֵ)
	FFramePackage GetFrameToCheck(ABlasterCharacter *HitCharacter, float HitTime);

	// ����HitTime��ӽ�����֡����Ϣ���в�ֵ
	FFramePackage InterpBetweenFrames(const FFramePackage &OlderFrame, const FFramePackage &YoungerFrame, const float HitTime);

	// ɨ����SSR����Ƿ����
	FServerSideRewindResult ConfirmHit(
		const FFramePackage &Package,
		ABlasterCharacter *HitCharacter,
		const FVector_NetQuantize &TraceStart,
		const FVector_NetQuantize &HitLocation
	);

	// ��ҩ��SSR����Ƿ����
	FServerSideRewindResult ProjectileConfirmHit(
		const FFramePackage &Package,
		ABlasterCharacter *HitCharacter,
		const FVector_NetQuantize &TraceStart,
		const FVector_NetQuantize100 InitialVelocity,
		float HitTime
	);

	// ����SSR����Ƿ����
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

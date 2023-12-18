// Fill out your copyright notice in the Description page of Project Settings.


#include "HitScanWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "WeaponTypes.h"
void AHitScanWeapon::Fire(const FVector &HitTarget)
{
	Super::Fire(HitTarget);

	APawn *OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr) return;
	AController *InstigatorController = OwnerPawn->GetController();
	const USkeletalMeshSocket *MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	if (MuzzleFlashSocket)
	{
		FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		FVector Start = SocketTransform.GetLocation();

		FHitResult FireHit;
		WeaponTraceHit(Start, HitTarget, FireHit);
		if (FireHit.bBlockingHit)
		{
			ABlasterCharacter *BlasterCharacter = Cast<ABlasterCharacter>(FireHit.GetActor());
			// 击中角色,造成伤害(只在服务器上造成伤害)
			if (BlasterCharacter && HasAuthority() && InstigatorController)
			{
				UGameplayStatics::ApplyDamage(
					BlasterCharacter,
					Damage,
					InstigatorController,
					this,
					UDamageType::StaticClass()
				);
			}
			// 击中时播放粒子效果
			if (ImpactParticles)
			{
				UGameplayStatics::SpawnEmitterAtLocation(
					GetWorld(),
					ImpactParticles,
					FireHit.ImpactPoint,
					FireHit.ImpactNormal.Rotation()
				);
			}
			if (HitSound)
			{
				UGameplayStatics::PlaySoundAtLocation(
					this,
					HitSound,
					FireHit.ImpactPoint
				);
			}
		}
		if (MuzzleFlash)
		{
			UGameplayStatics::SpawnEmitterAtLocation(
				GetWorld(),
				MuzzleFlash,
				SocketTransform
			);
		}
		if (FireSound)
		{
			UGameplayStatics::PlaySoundAtLocation(
				this,
				FireSound,
				GetActorLocation()
			);
		}
	}
}

void AHitScanWeapon::WeaponTraceHit(const FVector &TraceStart, const FVector &HitTarget, FHitResult &OutHit)
{
	FHitResult FireHit;

	UWorld *World = GetWorld();
	FVector End = bUseScatter ? TraceEndWithScatter(TraceStart, HitTarget) : TraceStart + (HitTarget - TraceStart) * 1.25f;

	if (World)
	{
		World->LineTraceSingleByChannel(
			OutHit,
			TraceStart,
			End,
			ECollisionChannel::ECC_Visibility
		);
		FVector BeamEnd = End;
		if (OutHit.bBlockingHit)
		{
			BeamEnd = OutHit.ImpactPoint;
		}
		if (BeamParticles)
		{
			UParticleSystemComponent *Beam = UGameplayStatics::SpawnEmitterAtLocation(
				World,
				BeamParticles,
				TraceStart,
				FRotator::ZeroRotator,
				true
			);
			if (Beam)
			{
				Beam->SetVectorParameter(FName("Target"), BeamEnd);
			}
		}
	}
}

FVector AHitScanWeapon::TraceEndWithScatter(const FVector &TraceStart, const FVector &HitTarget)
{
	// 从起点朝向瞄准的目标的向量
	FVector ToTargetNormalized = (HitTarget - TraceStart).GetSafeNormal();
	// 要散射的球的中心
	FVector SphereCenter = TraceStart + ToTargetNormalized * DistanceToSphere;
	// 从中心随机偏移的向量
	FVector RandVec = UKismetMathLibrary::RandomUnitVector() * FMath::FRandRange(0.f, SphereRadius);
	// 偏移后的终点位置
	FVector EndLoc = SphereCenter + RandVec;
	// 从起点朝向偏移后终点位置的向量
	FVector ToEndLocNormalized = (EndLoc - TraceStart).GetSafeNormal();

	// 测试用
	//// 画出散射的球
	//DrawDebugSphere(
	//	GetWorld(),
	//	SphereCenter,
	//	SphereRadius,
	//	20,
	//	FColor::Red,
	//	true
	//	);
	//// 画出偏移后的终点位置
	//DrawDebugSphere(
	//	GetWorld(),
	//	EndLoc,
	//	4.f,
	//	12,
	//	FColor::Orange,
	//	true
	//);
	//// 画出偏移后的LineTrace
	//DrawDebugLine(
	//	GetWorld(),
	//	TraceStart,
	//	FVector(TraceStart + ToEndLocNormalized * TRACE_LENGTH),
	//	FColor::Cyan,
	//	true
	//);
	// 返回LineTrace的终点 
	return FVector(TraceStart + ToEndLocNormalized * TRACE_LENGTH);
}




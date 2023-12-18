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
			// ���н�ɫ,����˺�(ֻ�ڷ�����������˺�)
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
			// ����ʱ��������Ч��
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
	// ����㳯����׼��Ŀ�������
	FVector ToTargetNormalized = (HitTarget - TraceStart).GetSafeNormal();
	// Ҫɢ����������
	FVector SphereCenter = TraceStart + ToTargetNormalized * DistanceToSphere;
	// ���������ƫ�Ƶ�����
	FVector RandVec = UKismetMathLibrary::RandomUnitVector() * FMath::FRandRange(0.f, SphereRadius);
	// ƫ�ƺ���յ�λ��
	FVector EndLoc = SphereCenter + RandVec;
	// ����㳯��ƫ�ƺ��յ�λ�õ�����
	FVector ToEndLocNormalized = (EndLoc - TraceStart).GetSafeNormal();

	// ������
	//// ����ɢ�����
	//DrawDebugSphere(
	//	GetWorld(),
	//	SphereCenter,
	//	SphereRadius,
	//	20,
	//	FColor::Red,
	//	true
	//	);
	//// ����ƫ�ƺ���յ�λ��
	//DrawDebugSphere(
	//	GetWorld(),
	//	EndLoc,
	//	4.f,
	//	12,
	//	FColor::Orange,
	//	true
	//);
	//// ����ƫ�ƺ��LineTrace
	//DrawDebugLine(
	//	GetWorld(),
	//	TraceStart,
	//	FVector(TraceStart + ToEndLocNormalized * TRACE_LENGTH),
	//	FColor::Cyan,
	//	true
	//);
	// ����LineTrace���յ� 
	return FVector(TraceStart + ToEndLocNormalized * TRACE_LENGTH);
}




// Fill out your copyright notice in the Description page of Project Settings.


#include "HitScanWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "WeaponTypes.h"
#include "Blaster/BlasterComponents/LagCompensationComponent.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
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
			// ���н�ɫ,����˺�
			if (BlasterCharacter && InstigatorController)
			{
				bool bCauseAuthDamage = !bUseServerSideRewind || OwnerPawn->IsLocallyControlled();
				// ���ʹ����SSR,�������ϵķǱ��ؿ��ƵĽ�ɫ�Ͳ�Ӧ�������һ���˺���
				if (HasAuthority() && bCauseAuthDamage)
				{
					const float DamageToCause = FireHit.BoneName.ToString() == FString("head") ? HeadShotDamage : Damage;

					UGameplayStatics::ApplyDamage(
						BlasterCharacter,
						DamageToCause,
						InstigatorController,
						this,
						UDamageType::StaticClass()
					);
				}
				if (!HasAuthority() && bUseServerSideRewind)
				{
					BlasterOwnerCharacter = BlasterOwnerCharacter == nullptr ? Cast<ABlasterCharacter>(OwnerPawn) : BlasterOwnerCharacter;
					BlasterOwnerController = BlasterOwnerController == nullptr ? Cast<ABlasterPlayerController>(InstigatorController) : BlasterOwnerController;
					if (BlasterOwnerCharacter && BlasterOwnerController && BlasterOwnerCharacter->GetLagCompensation() && BlasterOwnerCharacter->IsLocallyControlled())
					{
						BlasterOwnerCharacter->GetLagCompensation()->ServerScoreRequest(
							BlasterCharacter,
							Start,
							HitTarget,
							BlasterOwnerController->GetServerTime() - BlasterOwnerController->SingleTripTime
						);
					}
				}
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
	FVector End = TraceStart + (HitTarget - TraceStart) * 1.25f;

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
		else
		{
			OutHit.ImpactPoint = End;
		}
		//DrawDebugSphere(GetWorld(), BeamEnd, 16.f, 12, FColor::Orange, true);

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






// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include"Projectile.h"
void AProjectileWeapon::Fire(const FVector &HitTarget)
{
	Super::Fire(HitTarget);

	APawn *InstigatorPawn = Cast<APawn>(GetOwner());
	const USkeletalMeshSocket *MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));
	UWorld *World = GetWorld();

	if (MuzzleFlashSocket && World)
	{
		FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		// ��ǹ�ڵ�����λ�õķ���
		FVector ToTarget = HitTarget - SocketTransform.GetLocation();
		FRotator TargetRotation = ToTarget.Rotation();
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner();
		SpawnParams.Instigator = InstigatorPawn;

		AProjectile *SpawnedProjectile = nullptr;
		if (bUseServerSideRewind) // ʹ��SSR������
		{
			if (InstigatorPawn->HasAuthority()) // ������,ʹ��SSR
			{
				if (InstigatorPawn->IsLocallyControlled()) // �������Լ����ƵĽ�ɫ,����replicated���ӵ�, ��ʹ��SSR
				{
					SpawnedProjectile = World->SpawnActor<AProjectile>(ProjectileClass,SocketTransform.GetLocation(),TargetRotation,SpawnParams);
					SpawnedProjectile->bUseServerSideRewind = false;
					SpawnedProjectile->Damage = Damage;
					SpawnedProjectile->HeadShotDamage = HeadShotDamage;
				}
				else // �������ϷǷ������ɿ��ƵĽ�ɫ,����non-replicated���ӵ�, ʹ��SSR
				{
					SpawnedProjectile = World->SpawnActor<AProjectile>(NonReplicatedProjectileClass, SocketTransform.GetLocation(), TargetRotation, SpawnParams);
					SpawnedProjectile->bUseServerSideRewind = true;
				}
			}
			else // �ͻ���, ʹ��SSR
			{
				if (InstigatorPawn->IsLocallyControlled()) // �ͻ����Լ����ƵĽ�ɫ, ����non-replicated���ӵ�,ʹ��SSR
				{
					UE_LOG(LogTemp, Warning, TEXT("spawn ssr"));

					SpawnedProjectile = World->SpawnActor<AProjectile>(NonReplicatedProjectileClass, SocketTransform.GetLocation(), TargetRotation, SpawnParams);
					SpawnedProjectile->bUseServerSideRewind = true;
					SpawnedProjectile->TraceStart = SocketTransform.GetLocation();
					SpawnedProjectile->InitialVelocity = SpawnedProjectile->GetActorForwardVector() * SpawnedProjectile->InitialSpeed;
				}
				else // �ͻ��˷��Լ����ƵĽ�ɫ,����non-replicated���ӵ�,��ʹ��SSR
				{
					SpawnedProjectile = World->SpawnActor<AProjectile>(NonReplicatedProjectileClass, SocketTransform.GetLocation(), TargetRotation, SpawnParams);
					SpawnedProjectile->bUseServerSideRewind = false;
				}
			}
		}
		else // ��ʹ��SSR������
		{
			if (InstigatorPawn->HasAuthority())
			{
				SpawnedProjectile = World->SpawnActor<AProjectile>(ProjectileClass, SocketTransform.GetLocation(), TargetRotation, SpawnParams);
				SpawnedProjectile->bUseServerSideRewind = false;
				SpawnedProjectile->Damage = Damage;
				SpawnedProjectile->HeadShotDamage = HeadShotDamage;
			}
		}
	}
}

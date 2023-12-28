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
		// 从枪口到命中位置的方向
		FVector ToTarget = HitTarget - SocketTransform.GetLocation();
		FRotator TargetRotation = ToTarget.Rotation();
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner();
		SpawnParams.Instigator = InstigatorPawn;

		AProjectile *SpawnedProjectile = nullptr;
		if (bUseServerSideRewind) // 使用SSR的武器
		{
			if (InstigatorPawn->HasAuthority()) // 服务器,使用SSR
			{
				if (InstigatorPawn->IsLocallyControlled()) // 服务器自己控制的角色,发射replicated的子弹, 不使用SSR
				{
					SpawnedProjectile = World->SpawnActor<AProjectile>(ProjectileClass,SocketTransform.GetLocation(),TargetRotation,SpawnParams);
					SpawnedProjectile->bUseServerSideRewind = false;
					SpawnedProjectile->Damage = Damage;
					SpawnedProjectile->HeadShotDamage = HeadShotDamage;
				}
				else // 服务器上非服务器可控制的角色,发射non-replicated的子弹, 使用SSR
				{
					SpawnedProjectile = World->SpawnActor<AProjectile>(NonReplicatedProjectileClass, SocketTransform.GetLocation(), TargetRotation, SpawnParams);
					SpawnedProjectile->bUseServerSideRewind = true;
				}
			}
			else // 客户端, 使用SSR
			{
				if (InstigatorPawn->IsLocallyControlled()) // 客户端自己控制的角色, 发射non-replicated的子弹,使用SSR
				{
					UE_LOG(LogTemp, Warning, TEXT("spawn ssr"));

					SpawnedProjectile = World->SpawnActor<AProjectile>(NonReplicatedProjectileClass, SocketTransform.GetLocation(), TargetRotation, SpawnParams);
					SpawnedProjectile->bUseServerSideRewind = true;
					SpawnedProjectile->TraceStart = SocketTransform.GetLocation();
					SpawnedProjectile->InitialVelocity = SpawnedProjectile->GetActorForwardVector() * SpawnedProjectile->InitialSpeed;
				}
				else // 客户端非自己控制的角色,发射non-replicated的子弹,不使用SSR
				{
					SpawnedProjectile = World->SpawnActor<AProjectile>(NonReplicatedProjectileClass, SocketTransform.GetLocation(), TargetRotation, SpawnParams);
					SpawnedProjectile->bUseServerSideRewind = false;
				}
			}
		}
		else // 不使用SSR的武器
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

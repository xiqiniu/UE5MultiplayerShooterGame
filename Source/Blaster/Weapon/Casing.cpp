// Fill out your copyright notice in the Description page of Project Settings.


#include "Casing.h"
#include"Kismet/GameplayStatics.h"
#include"Sound/SoundCue.h"
ACasing::ACasing()
{
	PrimaryActorTick.bCanEverTick = true;

	CasingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CasingMesh"));
	SetRootComponent(CasingMesh);

	// ���ⵯ��Ӱ�����
	CasingMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	CasingMesh->SetSimulatePhysics(true);
	CasingMesh->SetEnableGravity(true);

	// ʹ��Physics��ʱ����Ҫ��������ȷ��Hit�¼��ܴ���
	CasingMesh->SetNotifyRigidBodyCollision(true);
	ShellEjectionImpulse = 10.f;

	ShellExpireTime = 5.f;
}

void ACasing::BeginPlay()
{
	Super::BeginPlay();
	CasingMesh->OnComponentHit.AddDynamic(this, &ACasing::OnHit);
	CasingMesh->AddImpulse(GetActorForwardVector() * ShellEjectionImpulse);
}

void ACasing::OnHit(UPrimitiveComponent *HitComp, AActor *OtherActor, UPrimitiveComponent *OtherComp, FVector NormalImpulse, const FHitResult &Hit)
{
	if (ShellSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ShellSound, GetActorLocation());
	}

	// �������������ȴ�һ��ʱ��������
	CasingMesh->SetNotifyRigidBodyCollision(false);
	FTimerHandle ShellExpireHandle;
	UWorld *World = GetWorld();
	if (World)
	{
		World->GetTimerManager().SetTimer(ShellExpireHandle, this, &ACasing::ShellExpire, ShellExpireTime, false);
	}
}

void ACasing::ShellExpire()
{
	Destroy();
}





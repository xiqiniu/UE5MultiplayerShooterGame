// Fill out your copyright notice in the Description page of Project Settings.


#include "BuffComponent.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "TimerManager.h"
#include "GameFramework/CharacterMovementComponent.h"

UBuffComponent::UBuffComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UBuffComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UBuffComponent::SetInitialSpeeds(float BaseSpeed, float CrouchSpeed)
{
	InitialBaseSpeed = BaseSpeed;
	InitialCrouchSpeed = CrouchSpeed;
}

void UBuffComponent::SetInitialJumpVelocity(float JumpVelocity)
{
	InitialJumpVelocity = JumpVelocity;
}

void UBuffComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	HealRampUp(DeltaTime);
	ShieldRampUp(DeltaTime);
}

void UBuffComponent::Heal(float HealAmount, float HealingTime)
{
	bHealing = true;
	HealingRate = HealAmount / HealingTime;
	AmountToHeal += HealAmount;
}

void UBuffComponent::ReplenishShield(float ShieldAmount, float ReplenishTime)
{
	bReplenishingShield = true;
	ShieldReplenishRate = ShieldAmount / ReplenishTime;
	ShieldReplenishAmount += ShieldAmount;
}

void UBuffComponent::HealRampUp(float DeltaTime)
{
	if (!bHealing || Character == nullptr || Character->IsElimmed()) return;

	const float HealThisFrame = HealingRate * DeltaTime;
	Character->SetHealth(FMath::Clamp(Character->GetHealth() + HealThisFrame, 0, Character->GetMaxHealth()));
	// 更新服务器上的血量显示,客户端的在OnRep_Health里已经更新了
	Character->UpdateHUDHealth();
	AmountToHeal -= HealThisFrame;
	if (AmountToHeal <= 0.f || Character->GetHealth() >= Character->GetMaxHealth())
	{
		bHealing = false;
		AmountToHeal = 0.f;
	}
}

void UBuffComponent::ShieldRampUp(float DeltaTime)
{
	if (!bReplenishingShield || Character == nullptr || Character->IsElimmed()) return;

	const float ReplenishThisFrame = ShieldReplenishRate * DeltaTime;
	Character->SetShield(FMath::Clamp(Character->GetShield() + ReplenishThisFrame, 0, Character->GetMaxShield()));
	// 更新服务器上的血量显示,客户端的在OnRep_Health里已经更新了
	Character->UpdateHUDShield();
	AmountToHeal -= ReplenishThisFrame;
	if (ShieldReplenishAmount <= 0.f || Character->GetShield() >= Character->GetMaxShield())
	{
		bReplenishingShield = false;
		ShieldReplenishAmount = 0.f;
	}
}

void UBuffComponent::BuffSpeed(float BuffBaseSpeed, float BuffCrouchSpeed, float BuffTime)
{
	if (Character == nullptr) return;

	Character->GetWorldTimerManager().SetTimer(
		SpeedBuffTimer,
		this,
		&UBuffComponent::ResetSpeeds,
		BuffTime
	);
	if (Character->GetCharacterMovement())
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = BuffBaseSpeed;
		Character->GetCharacterMovement()->MaxWalkSpeedCrouched = BuffCrouchSpeed;
	}
	// 通过广播同步客户端上的速度
	MulticastSpeedBuff(BuffBaseSpeed, BuffCrouchSpeed);

}

void UBuffComponent::BuffJump(float BuffJumpVelocity, float BuffTime)
{
	if (Character == nullptr) return;

	Character->GetWorldTimerManager().SetTimer(
		JumpBuffTimer,
		this,
		&UBuffComponent::ResetJump,
		BuffTime
	);
	if (Character->GetCharacterMovement())
	{
		Character->GetCharacterMovement()->JumpZVelocity = BuffJumpVelocity;
	}
	// 通过广播同步客户端上的速度
	MulticastJumpBuff(BuffJumpVelocity);
}

void UBuffComponent::ResetSpeeds()
{
	if (Character == nullptr || Character->GetCharacterMovement() == nullptr) return;
	Character->GetCharacterMovement()->MaxWalkSpeed = InitialBaseSpeed;
	Character->GetCharacterMovement()->MaxWalkSpeedCrouched = InitialCrouchSpeed;

	MulticastSpeedBuff(InitialBaseSpeed, InitialCrouchSpeed);
}

void UBuffComponent::ResetJump()
{
	if (Character == nullptr || Character->GetCharacterMovement() == nullptr) return;
	Character->GetCharacterMovement()->JumpZVelocity = InitialJumpVelocity;

	MulticastJumpBuff(InitialJumpVelocity);
}

void UBuffComponent::MulticastJumpBuff_Implementation(float JumpSpeed)
{
	if (Character == nullptr || Character->GetCharacterMovement() == nullptr) return;
	Character->GetCharacterMovement()->JumpZVelocity = JumpSpeed;
}

void UBuffComponent::MulticastSpeedBuff_Implementation(float BaseSpeed, float CrouchSpeed)
{
	if (Character == nullptr || Character->GetCharacterMovement() == nullptr) return;
	Character->GetCharacterMovement()->MaxWalkSpeed = BaseSpeed;
	Character->GetCharacterMovement()->MaxWalkSpeedCrouched = CrouchSpeed;
}



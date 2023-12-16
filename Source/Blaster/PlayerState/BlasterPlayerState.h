// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "BlasterPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API ABlasterPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;
	void ADDToScore(float ScoreAmount);
	void ADDToDefeats(int32 DefeatsAmount);

	virtual void OnRep_Score() override;

	UFUNCTION()
	virtual void OnRep_Defeats();

protected:

private:
	// ȷ��������ָ�뱻��ʼ��Ϊ��ָ��,�������ʹ��if�ж�������ָ���Ƿ�Ϊ�տ��ܻ������
	UPROPERTY()
	class ABlasterCharacter *Character;

	UPROPERTY()
	class ABlasterPlayerController *Controller;

	UPROPERTY(ReplicatedUsing = OnRep_Defeats)
	int32 Defeats;


};

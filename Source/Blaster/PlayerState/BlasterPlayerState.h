// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Blaster/BlasterTypes/Team.h"
#include "BlasterPlayerState.generated.h"

/**
 * �������ص�����: ����,��������,����
 */
UCLASS()
class BLASTER_API ABlasterPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;
	void ADDToScore(float ScoreAmount);
	void ADDToDefeats(int32 DefeatsAmount);

	// APlayerState �Ѿ��г�Ա����Score�ұ��Ϊ�ɸ��Ƶ�,ֱ����дOnRep_Score()
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
	
	UPROPERTY(ReplicatedUsing = OnRep_Team)
	ETeam Team = ETeam::ET_NoTeam;

	UFUNCTION()
	void OnRep_Team();

public:
	FORCEINLINE ETeam GetTeam() const { return Team; }
	void SetTeam(ETeam TeamToSet);
};

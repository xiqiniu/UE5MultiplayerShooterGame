// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Blaster/BlasterTypes/Team.h"
#include "BlasterPlayerState.generated.h"

/**
 * 与玩家相关的数据: 分数,死亡次数,队伍
 */
UCLASS()
class BLASTER_API ABlasterPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;
	void ADDToScore(float ScoreAmount);
	void ADDToDefeats(int32 DefeatsAmount);

	// APlayerState 已经有成员变量Score且标记为可复制的,直接重写OnRep_Score()
	virtual void OnRep_Score() override;

	UFUNCTION()
	virtual void OnRep_Defeats();

protected:

private:
	// 确保这两个指针被初始化为空指针,否则后续使用if判断这两个指针是否为空可能会出问题
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

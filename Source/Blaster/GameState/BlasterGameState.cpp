// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterGameState.h"
#include "Net/UnrealNetwork.h"
#include "Blaster/PlayerState/BlasterPlayerState.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
void ABlasterGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABlasterGameState, TopScoringPlayers);
	DOREPLIFETIME(ABlasterGameState, RedTeamScore);
	DOREPLIFETIME(ABlasterGameState, BlueTeamScore);
}

void ABlasterGameState::UpdateTopScore(ABlasterPlayerState *ScoringPlayer)
{
	// �߷����Ϊ��,ֱ�Ӽ���
	if (TopScoringPlayers.Num() == 0)
	{
		TopScoringPlayers.Add(ScoringPlayer);
		TopScore = ScoringPlayer->GetScore();
	}
	// ��߷���ͬ�����,������TArray,ʹ��AddUnique�������ͬһ�����
	else if (ScoringPlayer->GetScore() == TopScore)
	{
		TopScoringPlayers.AddUnique(ScoringPlayer);
	}
	else if (ScoringPlayer->GetScore() > TopScore)
	{
		// ���TArray
		TopScoringPlayers.Empty();
		TopScoringPlayers.AddUnique(ScoringPlayer);
		TopScore = ScoringPlayer->GetScore();
	}
}

void ABlasterGameState::RedTeamScores()
{
	++RedTeamScore;

	// �������ϸ��º�ӵ÷�
	ABlasterPlayerController *BlasterPlayerController = Cast<ABlasterPlayerController>(GetWorld()->GetFirstPlayerController());
	if (BlasterPlayerController)
	{
		BlasterPlayerController->SetHUDRedTeamScores(RedTeamScore);
	}
}

void ABlasterGameState::BlueTeamScores()
{
	++BlueTeamScore;

	ABlasterPlayerController *BlasterPlayerController = Cast<ABlasterPlayerController>(GetWorld()->GetFirstPlayerController());
	if (BlasterPlayerController)
	{
		BlasterPlayerController->SetHUDBlueTeamScores(BlueTeamScore);
	}
}

void ABlasterGameState::OnRep_RedTeamScore()
{
	// �ͻ����ϸ��º�ӵ÷�
	ABlasterPlayerController *BlasterPlayerController = Cast<ABlasterPlayerController>(GetWorld()->GetFirstPlayerController());
	if (BlasterPlayerController)
	{
		BlasterPlayerController->SetHUDRedTeamScores(RedTeamScore);
	}
}

void ABlasterGameState::OnRep_BlueTeamScore()
{
	ABlasterPlayerController *BlasterPlayerController = Cast<ABlasterPlayerController>(GetWorld()->GetFirstPlayerController());
	if (BlasterPlayerController)
	{
		BlasterPlayerController->SetHUDBlueTeamScores(BlueTeamScore);
	}
}

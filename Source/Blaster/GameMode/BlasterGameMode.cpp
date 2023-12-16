// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterGameMode.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "Blaster/PlayerState/BlasterPlayerState.h"
#include "Blaster/GameState/BlasterGameState.h"
namespace MatchState
{
    const FName Cooldown = FName("Cooldown");
}

ABlasterGameMode::ABlasterGameMode()
{
    // ����Ϸͣ��WaitingToStart���״̬,��״̬��GameMode���濴
    bDelayedStart = true;
}

void ABlasterGameMode::BeginPlay()
{
    LevelStartingTime = GetWorld()->GetTimeSeconds();
}

void ABlasterGameMode::OnMatchStateSet()
{
    Super::OnMatchStateSet();

    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        ABlasterPlayerController *BlasterPlayerController = Cast<ABlasterPlayerController>(*It);

        if (BlasterPlayerController)
        {
            BlasterPlayerController->OnMatchStateSet(MatchState);
        }
    }
}

void ABlasterGameMode::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (MatchState == MatchState::WaitingToStart)
    {
        // ֱ�Ӽ�ȥGetWorld()->GetTimeSeconds()���ȥ����ʱ��,��Ϊ�ӿͻ��˼�����Ϸ���ؿ�������ʼ��ʱ��Ҳ����ȥ��,Ҫ�ӻ���
        CountdownTime = WarmupTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
        if (CountdownTime <= 0.f)
        {
            // StartMatch ����MatchStateת��ΪInProgress
            StartMatch();
        }
    }
    else if (MatchState == MatchState::InProgress)
    {
        CountdownTime = WarmupTime + MatchTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
        if (CountdownTime <= 0.f)
        {
            // �ı�MatchState
            SetMatchState(MatchState::Cooldown);
        }
    }
    else if (MatchState == MatchState::Cooldown)
    {
        CountdownTime = CooldownTime + WarmupTime + MatchTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
        if (CountdownTime <= 0.f)
        {
            RestartGame();
        }
    }
}

void ABlasterGameMode::PlayerEliminated(ABlasterCharacter *ElimmedCharacter, ABlasterPlayerController *VictimController, ABlasterPlayerController *AttackerController)
{
    ABlasterPlayerState *AttackPlayerState = AttackerController ? Cast<ABlasterPlayerState>(AttackerController->PlayerState) : nullptr;
    ABlasterPlayerState *VictimPlayerState = VictimController ? Cast<ABlasterPlayerState>(VictimController->PlayerState) : nullptr;

    ABlasterGameState *BlasterGameState = GetGameState<ABlasterGameState>();

    if (AttackPlayerState && AttackPlayerState != VictimPlayerState && BlasterGameState)
    {
        AttackPlayerState->ADDToScore(1.f);
        BlasterGameState->UpdateTopScore(AttackPlayerState);
    }
    if (VictimPlayerState)
    {
        VictimPlayerState->ADDToDefeats(1);
    }
	if (ElimmedCharacter)
	{
		ElimmedCharacter->Elim();
	}
}

void ABlasterGameMode::RequestRespawn(ACharacter *ElimmedCharacter, AController *ElimmedController)
{
	if (ElimmedCharacter)
	{
		ElimmedCharacter->Reset();
		ElimmedCharacter->Destroy();
	}
	if (ElimmedController)
	{
         //�ҵ��������Զ�ĳ���������
        // ��ȡ������ҽ�ɫ��λ��
        TArray<FVector> PlayerPositions;
        AGameStateBase *CurrentGameState = GetGameState<AGameStateBase>();
        if (CurrentGameState)
        {
            for (APlayerState *PlayerState : CurrentGameState->PlayerArray)
            {
                ACharacter *PlayerCharacter = Cast<ACharacter>(PlayerState->GetPawn());
                if (PlayerCharacter)
                {
                    PlayerPositions.Add(PlayerCharacter->GetActorLocation());
                }
            }

            // ��ȡ����������
            TArray<AActor *> PlayerStartArray;
            UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStartArray);

            // �ҵ������������Զ��������
            AActor *BestSpawn = nullptr;
            float MaxDistance = -1.0f;

            for (AActor *SpawnPoint : PlayerStartArray)
            {
                float MinDistanceToPlayer = FLT_MAX;

                // �ҳ��뵱ǰ��������������
                for (const FVector &PlayerPos : PlayerPositions)
                {
                    float Distance = FVector::Dist(SpawnPoint->GetActorLocation(), PlayerPos);
                    if (Distance < MinDistanceToPlayer)
                    {
                        MinDistanceToPlayer = Distance;
                    }
                }

                // ���������������Զ��������
                if (MinDistanceToPlayer > MaxDistance)
                {
                    MaxDistance = MinDistanceToPlayer;
                    BestSpawn = SpawnPoint;
                }
            }

            // ʹ��ѡ����������������ɫ
            if (BestSpawn)
            {
                RestartPlayerAtPlayerStart(ElimmedController, BestSpawn);
            }
        }
        // �������
		//TArray<AActor *> PlayerStartArray;
		//UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStartArray);
		//int32 Selection = FMath::RandRange(0, PlayerStartArray.Num() - 1);
		//RestartPlayerAtPlayerStart(ElimmedController, PlayerStartArray[Selection]);
	}
}


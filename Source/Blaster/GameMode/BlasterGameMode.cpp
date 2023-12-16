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
    // 让游戏停在WaitingToStart这个状态,各状态进GameMode里面看
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
        // 直接减去GetWorld()->GetTimeSeconds()会减去过多时间,因为从客户端加入游戏到关卡真正开始的时间也被减去了,要加回来
        CountdownTime = WarmupTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
        if (CountdownTime <= 0.f)
        {
            // StartMatch 会让MatchState转变为InProgress
            StartMatch();
        }
    }
    else if (MatchState == MatchState::InProgress)
    {
        CountdownTime = WarmupTime + MatchTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
        if (CountdownTime <= 0.f)
        {
            // 改变MatchState
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
         //找到离玩家最远的出生点重生
        // 获取所有玩家角色的位置
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

            // 获取所有重生点
            TArray<AActor *> PlayerStartArray;
            UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStartArray);

            // 找到离所有玩家最远的重生点
            AActor *BestSpawn = nullptr;
            float MaxDistance = -1.0f;

            for (AActor *SpawnPoint : PlayerStartArray)
            {
                float MinDistanceToPlayer = FLT_MAX;

                // 找出离当前重生点最近的玩家
                for (const FVector &PlayerPos : PlayerPositions)
                {
                    float Distance = FVector::Dist(SpawnPoint->GetActorLocation(), PlayerPos);
                    if (Distance < MinDistanceToPlayer)
                    {
                        MinDistanceToPlayer = Distance;
                    }
                }

                // 更新离所有玩家最远的重生点
                if (MinDistanceToPlayer > MaxDistance)
                {
                    MaxDistance = MinDistanceToPlayer;
                    BestSpawn = SpawnPoint;
                }
            }

            // 使用选定的重生点重生角色
            if (BestSpawn)
            {
                RestartPlayerAtPlayerStart(ElimmedController, BestSpawn);
            }
        }
        // 随机重生
		//TArray<AActor *> PlayerStartArray;
		//UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStartArray);
		//int32 Selection = FMath::RandRange(0, PlayerStartArray.Num() - 1);
		//RestartPlayerAtPlayerStart(ElimmedController, PlayerStartArray[Selection]);
	}
}


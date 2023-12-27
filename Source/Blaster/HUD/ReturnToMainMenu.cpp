// Fill out your copyright notice in the Description page of Project Settings.


#include "ReturnToMainMenu.h"
#include "GameFramework/PlayerController.h"
#include "Components/Button.h"
#include "MultiplayerSessionsSubsystem.h"
#include "GameFramework/GameModeBase.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Blaster/PlayerController/LobbyPlayerController.h"
#include "Blaster/Blaster.h"
// ��ʾ���ز˵���ť,�����������ģʽ,�󶨰�ť���,����ϵͳ��OnDestroyί��
void UReturnToMainMenu::MenuSetup()
{
	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	bIsFocusable = true;
	UWorld *World = GetWorld();
	if (World)
	{
		PlayerController = PlayerController == nullptr ? World->GetFirstPlayerController() : PlayerController;
		if (PlayerController)
		{
			FInputModeGameAndUI InputModeData;
			InputModeData.SetWidgetToFocus(TakeWidget());
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(true);
		}
	}

	if (ReturnButton && !ReturnButton->OnClicked.IsBound())
	{
		ReturnButton->OnClicked.AddDynamic(this, &UReturnToMainMenu::ReturnButtonClicked);
	}

	UGameInstance *GameInstance = GetGameInstance();
	if (GameInstance)
	{
		MultiplayerSessionsSubsystem = MultiplayerSessionsSubsystem == nullptr ?
			GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>() : MultiplayerSessionsSubsystem;
		if (MultiplayerSessionsSubsystem)
		{
			MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.AddDynamic(this, &UReturnToMainMenu::OnDestroySession);
		}
	}
}

// ��ֹ��ʾ���ز˵���ť,�����������ģʽ,�Ƴ�֮ǰ�İ�
void UReturnToMainMenu::MenuTearDown()
{
	RemoveFromParent();
	UWorld *World = GetWorld();
	if (World)
	{
		PlayerController = PlayerController == nullptr ? World->GetFirstPlayerController() : PlayerController;
		if (PlayerController)
		{
			FInputModeGameOnly InputModeData;
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(false);
		}
	}
	if (ReturnButton && ReturnButton->OnClicked.IsBound())
	{
		ReturnButton->OnClicked.RemoveDynamic(this, &UReturnToMainMenu::ReturnButtonClicked);
	}
	if (MultiplayerSessionsSubsystem && MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.IsBound())
	{
		MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.RemoveDynamic(this, &UReturnToMainMenu::OnDestroySession);
	}
}

bool UReturnToMainMenu::Initialize()
{
	if (!Super::Initialize()) return false;
	return true;
}

// ���ڰ󶨵���ť����¼�,��ҵ����:
// 1. ���ð�ť 
// 2. �����ҽ�ɫ����,����ServerLeaveGame()����OnLeftGame, �����������ð�ť
// ServerLeaveGame�����BlasterGameMode��PlayerLeftGame������ҽ�ɫ��Ӧ��PlayerState
// PlayerLeftGame��,�����ǰ����˳���ť���������߷����֮һ,�����Ƴ��߷��������,
// ֮������PlayerState��ȡ����ɫ����ٵ��ý�ɫ���Elim������true
// Elim�����MulticastElim�ڴ���������������߼���Ὺ��һ����ʱ��
// ����ʱ���󶨵ĺ����л����Elim�����boolֵ����������������һ��Ǵ���OnLeftGame�Ĺ㲥
// OnLeftGame�����������OnPlayerLeftGame,���������MultiplayerSessionsSubsystem��DestroySession�������ٻỰ
// ���ٽ����������MenuSetup����󶨵�OnDestroySession,�������Ƿ��������ǿͻ��˵�����Ӧ�ĺ����ص����˵�
void UReturnToMainMenu::ReturnButtonClicked()
{
	ReturnButton->SetIsEnabled(false);
	UWorld *World = GetWorld();
	if (World)
	{
		APlayerController *FirstPlayerController = World->GetFirstPlayerController();
		if (FirstPlayerController)
		{
			ABlasterCharacter *BlasterCharacter = Cast<ABlasterCharacter>(FirstPlayerController->GetPawn());
			ABlasterPlayerController *BlasterPlayerController = Cast<ABlasterPlayerController>(FirstPlayerController);
			ALobbyPlayerController *LobbyPlayerController = Cast<ALobbyPlayerController>(FirstPlayerController);
			if (LobbyPlayerController)
			{
				LogOnScreen(this, "Call OnPlayerLeftGame()");
				OnPlayerLeftGame();
			}
			else if (BlasterCharacter)
			{
				BlasterCharacter->ServerLeaveGame();
				BlasterCharacter->OnLeftGame.AddDynamic(this, &UReturnToMainMenu::OnPlayerLeftGame);
			}
			else if(BlasterPlayerController)
			{
				LogOnScreen(this, "Call BlasterPlayerController->ServerLeaveGame()");
				BlasterPlayerController->OnLeftGameByController.AddDynamic(this, &UReturnToMainMenu::OnPlayerLeftGame);
				BlasterPlayerController->ServerLeaveGame();
			}
			else
			{
				ReturnButton->SetIsEnabled(true);
			}
		}
	}
}

void UReturnToMainMenu::OnPlayerLeftGame()
{
	LogOnScreen(this, "OnPlayerLeftGame() Get Called");

	UGameInstance *GameInstance = GetGameInstance();
	if (GameInstance)
	{
		MultiplayerSessionsSubsystem = MultiplayerSessionsSubsystem == nullptr ?
			GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>() : MultiplayerSessionsSubsystem;
		if (MultiplayerSessionsSubsystem)
		{
			MultiplayerSessionsSubsystem->DestroySession();
		}
	}
}

void UReturnToMainMenu::OnDestroySession(bool bWasSuccessful)
{
	LogOnScreen(this, "OnDestroySession Get Called");

	if (!bWasSuccessful)
	{
		ReturnButton->SetIsEnabled(true);
		return;
	}
	UWorld *World = GetWorld();
	if (World)
	{
		// �������ص����˵�
		AGameModeBase *GameMode =World->GetAuthGameMode<AGameModeBase>();
		if (GameMode)
		{
			GameMode->ReturnToMainMenuHost();
		}
		else // �ͻ��˻ص����˵� 
		{
			PlayerController = PlayerController == nullptr ? World->GetFirstPlayerController() : PlayerController;
			if (PlayerController)
			{
				PlayerController->ClientReturnToMainMenuWithTextReason(FText());
			}
		}
	}
}



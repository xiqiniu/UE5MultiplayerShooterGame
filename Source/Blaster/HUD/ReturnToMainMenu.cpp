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
// 显示返回菜单按钮,更改玩家输入模式,绑定按钮点击,绑定子系统的OnDestroy委托
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

// 禁止显示返回菜单按钮,更改玩家输入模式,移除之前的绑定
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

// 用于绑定到按钮点击事件,玩家点击后:
// 1. 禁用按钮 
// 2. 如果玩家角色存在,调用ServerLeaveGame()并绑定OnLeftGame, 否则重新启用按钮
// ServerLeaveGame会调用BlasterGameMode的PlayerLeftGame传入玩家角色对应的PlayerState
// PlayerLeftGame中,如果当前点击退出按钮的玩家是最高分玩家之一,将其移出高分玩家数组,
// 之后利用PlayerState获取到角色类后再调用角色类的Elim并传入true
// Elim会调用MulticastElim在处理完玩家死亡的逻辑后会开启一个定时器
// 而定时器绑定的函数中会根据Elim传入的bool值决定是重新生成玩家还是触发OnLeftGame的广播
// OnLeftGame被触发后调用OnPlayerLeftGame,里面调用了MultiplayerSessionsSubsystem的DestroySession函数销毁会话
// 销毁结束后调用在MenuSetup里面绑定的OnDestroySession,并根据是服务器还是客户端调用相应的函数回到主菜单
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
		// 服务器回到主菜单
		AGameModeBase *GameMode =World->GetAuthGameMode<AGameModeBase>();
		if (GameMode)
		{
			GameMode->ReturnToMainMenuHost();
		}
		else // 客户端回到主菜单 
		{
			PlayerController = PlayerController == nullptr ? World->GetFirstPlayerController() : PlayerController;
			if (PlayerController)
			{
				PlayerController->ClientReturnToMainMenuWithTextReason(FText());
			}
		}
	}
}



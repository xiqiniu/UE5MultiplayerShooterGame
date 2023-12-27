// Fill out your copyright notice in the Description page of Project Settings.


#include "OverHeadWidget.h"
#include"Components/TextBlock.h"
void UOverHeadWidget::SetDisplayText(FString TextToDisplay)
{
	if (DisplayText)
	{
		DisplayText->SetText(FText::FromString(TextToDisplay));
	}
}

void UOverHeadWidget::ShowPlayerNetRole(APawn *InPawn)
{
	//ENetRole LocalRole = InPawn->GetLocalRole();
	//FString Role;
	//switch (LocalRole)
	//{
	//case ENetRole::ROLE_Authority :
	//	Role = FString("Authority");
	//	break;
	//case ENetRole::ROLE_AutonomousProxy:
	//	Role = FString("Autonomous Proxy");
	//	break;
	//case ENetRole::ROLE_SimulatedProxy:
	//	Role = FString("Simulated Proxy");
	//	break;
	//case ENetRole::ROLE_None:
	//	Role = FString("None");
	//	break;
	//}
	//FString LocalRoleString = FString::Printf(TEXT("Local Role: %s"), *Role);
	//SetDisplayText(LocalRoleString);

	ENetRole RemoteRole = InPawn->GetRemoteRole();
	FString Role;
	switch (RemoteRole)
	{
	case ENetRole::ROLE_Authority:
		Role = FString("Authority");
		break;
	case ENetRole::ROLE_AutonomousProxy:
		Role = FString("Autonomous Proxy");
		break;
	case ENetRole::ROLE_SimulatedProxy:
		Role = FString("Simulated Proxy");
		break;
	case ENetRole::ROLE_None:
		Role = FString("None");
		break;
	}
	FString RemoteRoleString = FString::Printf(TEXT("Remote Role: %s"), *Role);
	SetDisplayText(RemoteRoleString);
}

void UOverHeadWidget::NativeDestruct()
{
	RemoveFromParent();
	Super::NativeDestruct();
}

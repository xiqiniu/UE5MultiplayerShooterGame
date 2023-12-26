// Fill out your copyright notice in the Description page of Project Settings.


#include "ElimAnnouncement.h"
#include "Components/TextBlock.h"

void UElimAnnouncement::SetElimAnnouncementText(FText AttackerName, FText VictimName)
{
	FText ElimAnnouncementText = FText::Format(
		NSLOCTEXT("123", "123", "{0} 消灭了 {1}!"),
		AttackerName,
		VictimName
	);

	if (AnnouncementText)
	{
		AnnouncementText->SetText(ElimAnnouncementText);
	}
}

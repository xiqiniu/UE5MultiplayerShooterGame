// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BlasterHUD.generated.h"

USTRUCT(BlueprintType)
struct FHUDPackage
{
	GENERATED_BODY()
public:
	class UTexture2D *CrosshairsCenter;
	class UTexture2D *CrosshairsLeft;
	class UTexture2D *CrosshairsRight;
	class UTexture2D *CrosshairsTop;
	class UTexture2D *CrosshairsBottom;
	float CrosshairSpread;
	FLinearColor CrosshairsColor;

};
/**
 * 主HUD
 */
UCLASS()
class BLASTER_API ABlasterHUD : public AHUD
{
	GENERATED_BODY()
public:
	virtual void DrawHUD() override;

	UPROPERTY(EditAnywhere, Category = "Player Stats")
	TSubclassOf<class UUserWidget> CharacterOverlayClass;

	UPROPERTY()
	class UCharacterOverlay *CharacterOverlay;

	void AddCharacterOverlay();
	  
	UPROPERTY(EditAnywhere, Category = "Announcements")
	TSubclassOf<UUserWidget> AnnouncementClass;

	UPROPERTY()
	class UAnnouncement *Announcement;

	void AddAnnouncement();

	void AddElimAnnouncement(FText Attacker, FText Victim);

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	class APlayerController *OwningPlayer;

	FHUDPackage HUDPackage;

	void DrawCrosshair(UTexture2D *Texture, FVector2D ViewportCenter, FVector2D Spread, FLinearColor CrosshairsColor);

	UPROPERTY(EditAnywhere)
	float CrosshairSpreadMax = 16.f;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UElimAnnouncement> ElimAnnouncementClass;

	UPROPERTY(EditAnywhere)
	float ElimAnnouncementTime = 5.f;

	UFUNCTION()
	void ElimAnnouncementTimerFinished(UElimAnnouncement *MsgToRemove);

	// 击杀信息,每个信息会持续一段时间,所以要用TArray存下来
	UPROPERTY()
	TArray<UElimAnnouncement *>ElimMessages;

public:
	FORCEINLINE void SetHUDPackage(const FHUDPackage &Package) { HUDPackage = Package; }
};

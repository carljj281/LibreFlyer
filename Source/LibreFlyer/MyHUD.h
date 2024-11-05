// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MyHUD.generated.h"

class SConstraintCanvas;
class SQuadcopterSettingsWidget;
class UInputComponent;
class UTextureRenderTarget2D;
class UUserWidget;

class LIBREFLYER_API SCheckpointSplit : public SCompoundWidget {
public:
	SLATE_BEGIN_ARGS(SCheckpointSplit) {
		}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;


	TSharedPtr<SBorder> CheckpointSplitBorder;
	TSharedPtr<STextBlock> CheckpointSplitText;
	TSharedPtr<FNumberFormattingOptions> NumberFormattingOptions;

	void UpdateCheckpointSplit(float SplitTime);
	bool bStartFading;
	float OpacityTicker;
};

UCLASS()
class LIBREFLYER_API AMyHUD : public AHUD
{
	GENERATED_BODY()
public:
	AMyHUD();
	void Tick(float DeltaTime) override;
	void DrawHUD() override;

	void CreateHud();


	TSharedPtr<SConstraintCanvas> PlayerSlateHud;

	TSharedPtr<STextBlock> FpsDisplay;
	TSharedPtr<SCheckpointSplit> CheckpointSplit;

	float FpsBuffer;
	float FpsSamples;
	FNumberFormattingOptions NumberFormat;

	bool bShowCheckpointMarkers;
	FVector2D RectagleSize;

	void ToggleEscapeMenu();
	bool bEscapeMenuIsOpen;
	UUserWidget* EscapeMenu;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UUserWidget> EscapeMenuClass;


	void DrawWaypoint(const FRay& CheckpointRay, bool bIsGrey = false);
	void OnCheckpointTriggered(FRay& NewWaypoint);
	FRay CurrentCheckpointRay;
	FRay NextCheckpointRay;

	void OnResetTrack(FRay& FirstCheckpointRay, FRay& SecondCheckpointRay);
};

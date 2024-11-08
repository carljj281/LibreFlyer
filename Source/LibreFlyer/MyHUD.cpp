// Fill out your copyright notice in the Description page of Project Settings.


#include "MyHUD.h"

#include "Widgets/Layout/SConstraintCanvas.h"
#include "Engine/Canvas.h"
#include "SQuadcopterSettingsWidget.h"
#include "Quadcopter.h"
#include "SlateOptMacros.h"
#include "Blueprint/UserWidget.h"
#include "UnrealEngine.h"

AMyHUD::AMyHUD() {
	bAllowTickBeforeBeginPlay = false;
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	SetActorTickEnabled(false);

	FpsBuffer = 0.f;
	FpsSamples = 0.f;
	NumberFormat.MinimumFractionalDigits = 4;
	NumberFormat.UseGrouping = false;
	bShowCheckpointMarkers = false;
	RectagleSize = FVector2D(20.f);
	bEscapeMenuIsOpen = false;
}
void AMyHUD::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	extern ENGINE_API float GAverageMS;
	FpsBuffer += GAverageMS;
	FpsSamples++;
	if (FpsBuffer > 50.f) {
		FpsDisplay->SetText(FText::FromString(FText::AsNumber(1000.f / (FpsBuffer / FpsSamples), &NumberFormat).ToString() + " FPS"));
		FpsBuffer = 0.f;
		FpsSamples = 0;
	}
}
void AMyHUD::DrawHUD() {
	Super::DrawHUD();
	if (bShowCheckpointMarkers) {
		DrawWaypoint(CurrentCheckpointRay);
		DrawWaypoint(NextCheckpointRay, true);
	}
}
void AMyHUD::CreateHud() {
	const auto FontPath = FPaths::Combine(FPaths::EngineContentDir() / TEXT("Slate") / TEXT("Fonts") / "Roboto-Light.ttf");
	const auto WhiteTexturePath = FPaths::Combine(FPaths::EngineContentDir() / TEXT("Slate") / TEXT("Fonts") / "Roboto-Light.ttf");
	const auto BorderBrush = new FSlateBrush();
	BorderBrush->TintColor = FLinearColor::White;
	auto MediumHudFont = FSlateFontInfo(FontPath, 20);
	MediumHudFont.OutlineSettings.OutlineSize = 1;

	InputComponent->BindAction("EscapeMenu", IE_Pressed, this, &AMyHUD::ToggleEscapeMenu);
	// canvas for all hud widgets
	GEngine->GameViewport->AddViewportWidgetForPlayer(
		GetOwningPlayerController()->GetLocalPlayer(),
		SAssignNew(PlayerSlateHud, SConstraintCanvas),
		0
	);
	// fps display
	PlayerSlateHud->AddSlot()
		.Anchors(FAnchors(0.f, 0.f, 0.f, 0.f))
		.Alignment(FVector2D(0.f, 0.f))
		.AutoSize(true)
		[
			SNew(SBorder)
				.BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
				.Padding(0.f)
				.BorderBackgroundColor(FLinearColor(0.02f, 0.02f, 0.02f, 1.f))
				[
					SAssignNew(FpsDisplay, STextBlock)
						.Font(MediumHudFont)
						.ColorAndOpacity(FLinearColor(0.7f, 0.7f, 0.7f, 1.f))
				]
		];
	// checkpoint split
	PlayerSlateHud->AddSlot()
		.Anchors(FAnchors(0.5f, 0.5f, 0.5f, 0.5f))
		.Alignment(FVector2D(0.5f, -3.5f))
		.AutoSize(true)
		[
			SAssignNew(CheckpointSplit, SCheckpointSplit)
		];
}
void AMyHUD::DrawWaypoint(const FRay& CheckpointRay, bool bIsGrey) {
	// project checkpoint location to screen
	auto ScreenSpaceVector = Project(CheckpointRay.Origin);
	// if checkpoint is in front of the player...
	if (ScreenSpaceVector.Z != 0.f) {
		// determine marker's color
		bool bIsGreen = false;
		if (!bIsGrey) {
			auto DotProduct = FVector::DotProduct(
				CheckpointRay.Origin - GetOwningPawn()->GetActorLocation(),
				CheckpointRay.Direction
			);
			bIsGreen = (DotProduct > 0.f);
		}
		// draw checkpoint marker
		DrawRect(
			bIsGrey ? FLinearColor::Gray : bIsGreen ? FLinearColor::Green : FLinearColor::Red,
			ScreenSpaceVector.X - RectagleSize.X * 0.5f, ScreenSpaceVector.Y - RectagleSize.Y * 0.5f,
			RectagleSize.X,
			RectagleSize.Y
		);
	}
}
void AMyHUD::ToggleEscapeMenu() {
	if (bEscapeMenuIsOpen) {
		// hide menu and enable input, hide mouse cursor
		EscapeMenu->SetVisibility(ESlateVisibility::Hidden);
		GetOwningPlayerController()->bShowMouseCursor = false;
		GetOwningPlayerController()->SetInputMode(FInputModeGameOnly());
	}
	else {
		if (!EscapeMenu) {
			if (GetOwningPawn()) {
				if (auto Quadcopter = Cast<AQuadcopter>(GetOwningPawn())) {
					// create escape menu
					EscapeMenu = UUserWidget::CreateWidgetInstance(*GetWorld(), EscapeMenuClass, "EscapeMenu");
					EscapeMenu->AddToPlayerScreen();
				}
			}
		}
		else {
			// unhide escape menu
			EscapeMenu->SetVisibility(ESlateVisibility::Visible);
		}
		// disable input, show mouse cursor
		GetOwningPlayerController()->bShowMouseCursor = true;
		GetOwningPlayerController()->SetInputMode(FInputModeGameAndUI());
	}
	bEscapeMenuIsOpen = !bEscapeMenuIsOpen;
}
BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SCheckpointSplit::Construct(const FArguments& InArgs) {
	bStartFading = false;
	NumberFormattingOptions = MakeShareable(new FNumberFormattingOptions());
	NumberFormattingOptions->SetUseGrouping(false);
	NumberFormattingOptions->SetAlwaysSign(false);

	const auto FontPath = FPaths::Combine(FPaths::EngineContentDir() / TEXT("Slate") / TEXT("Fonts") / "Roboto-Regular.ttf");
	auto MediumHudFont = FSlateFontInfo(FontPath, 22);
	MediumHudFont.OutlineSettings.OutlineSize = 1;

	ChildSlot
		[
			SAssignNew(CheckpointSplitBorder, SBorder)
				.BorderImage(new FSlateBrush())
				.BorderBackgroundColor(FLinearColor::Transparent)
				[
					SAssignNew(CheckpointSplitText, STextBlock)
						.Font(MediumHudFont)
						.Margin(0.f)
						.ColorAndOpacity(FLinearColor(0.7f, 0.7f, 0.7f, 1.f))
				]
		];
}
void SCheckpointSplit::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) {
	if (bStartFading) {
		OpacityTicker -= InDeltaTime * 4.f;
		if (OpacityTicker < 0.f) {
			OpacityTicker = 0.f;
			bStartFading = false;
			SetRenderOpacity(OpacityTicker);
		}
		else if (OpacityTicker < 1.f) {
			SetRenderOpacity(OpacityTicker);
		}
	}
	SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION


void SCheckpointSplit::UpdateCheckpointSplit(float SplitTime) {
	auto TimespanFormatPattern = FText::FromString("{Sign}{Minutes}:{Seconds}.{Milliseconds}");

	FFormatNamedArguments TimeArguments;
	const auto bIsPositive = (SplitTime > 0);
	TimeArguments.Add(TEXT("Sign"), FText::FromString(bIsPositive ? "+" : "-"));
	SplitTime = FMath::Abs(SplitTime);
	const auto Minutes = (SplitTime > 60.f) ? FMath::TruncToInt(SplitTime / 60.0f) : 0.f;
	NumberFormattingOptions->SetMinimumIntegralDigits(2);
	NumberFormattingOptions->SetMaximumIntegralDigits(2);
	TimeArguments.Add(TEXT("Minutes"), FText::AsNumber(Minutes, NumberFormattingOptions.Get()));
	TimeArguments.Add(TEXT("Seconds"), FText::AsNumber(FMath::TruncToInt(SplitTime) % 60, NumberFormattingOptions.Get()));
	NumberFormattingOptions->SetMinimumIntegralDigits(4);
	NumberFormattingOptions->SetMaximumIntegralDigits(4);
	TimeArguments.Add(TEXT("Milliseconds"), FText::AsNumber(FMath::TruncToInt(FMath::Fractional(SplitTime) * 10000), NumberFormattingOptions.Get()));
	CheckpointSplitText->SetText(FText::Format(TimespanFormatPattern, TimeArguments));
	auto BorderColor = bIsPositive ? FLinearColor(0.4f, 0.f, 0.f, 0.5f) : FLinearColor(0.f, 0.4f, 0.f, 0.5f);
	CheckpointSplitBorder->SetBorderBackgroundColor(BorderColor);
}
void AMyHUD::OnCheckpointTriggered(FRay& NewWaypoint) {
	CurrentCheckpointRay = NextCheckpointRay;
	NextCheckpointRay = NewWaypoint;
}

void AMyHUD::OnResetTrack(FRay& FirstCheckpointRay, FRay& SecondCheckpointRay) {
	CurrentCheckpointRay = FirstCheckpointRay;
	NextCheckpointRay = SecondCheckpointRay;
	bShowCheckpointMarkers = true;
	SetActorTickEnabled(true);
}

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MyGameModeBase.generated.h"

class AMyGameStateBase;

UCLASS()
class LIBREFLYER_API AMyGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:

	AMyGameModeBase();
	void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	void PostBeginPlay();
	void ResetTrack();
	AMyGameStateBase* MyGameState;
};

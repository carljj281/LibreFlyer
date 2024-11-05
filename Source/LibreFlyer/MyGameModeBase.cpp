// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameModeBase.h"
#include "MyPlayerState.h"
#include "MyHUD.h"
#include "MyGameStateBase.h"
#include "EngineUtils.h"
#include "Checkpoint.h"
#include "GameFramework/PlayerStart.h"
#include "Net/Core/PushModel/PushModel.h"
#include "Quadcopter.h"

AMyGameModeBase::AMyGameModeBase() {
	PlayerStateClass = AMyPlayerState::StaticClass();
	HUDClass = AMyHUD::StaticClass();
	GameStateClass = AMyGameStateBase::StaticClass();
	DefaultPawnClass = AQuadcopter::StaticClass();
}
void AMyGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) {
	Super::InitGame(MapName, Options, ErrorMessage);
	GetWorld()->OnWorldBeginPlay.AddUObject(this, &AMyGameModeBase::PostBeginPlay);
}
void AMyGameModeBase::PostBeginPlay() {
	MyGameState = GetGameState<AMyGameStateBase>();
	ResetTrack();
}
void AMyGameModeBase::ResetTrack() {
	auto World = GetWorld();

	MyGameState->Checkpoints.Empty();
	MyGameState->StartingBlocks.Empty();
	// gather starting blocks
	for (TActorIterator<APlayerStart> i(World); i; ++i) {
		MyGameState->StartingBlocks.Add(*i);
	}
	// gather all checkpoints, and add to array
	for (TActorIterator<ACheckpoint> i(World); i; ++i) {
		MyGameState->Checkpoints.Add(*i);
	}
	MyGameState->Checkpoints.Sort();
	for (TActorIterator<AMyPlayerState> i(World); i; ++i) {
		i->CurrentCheckpointIndex = 0;
		i->NextCheckpointIndex = 1;
	}
	MARK_PROPERTY_DIRTY_FROM_NAME(AMyGameStateBase, Checkpoints, MyGameState);
	MARK_PROPERTY_DIRTY_FROM_NAME(AMyGameStateBase, StartingBlocks, MyGameState);

	MyGameState->bResetTrack = true;
	MyGameState->bReceivedCheckpoints = true;
	MyGameState->bReceivedStartingBlocks = true;
	MyGameState->OnRep_bResetTrack();
	
}
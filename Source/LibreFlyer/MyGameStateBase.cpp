// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameStateBase.h"
#include "EngineUtils.h"
#include "Quadcopter.h"
#include "MyPlayerState.h"
#include "MyHUD.h"
#include "Checkpoint.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"
#include "PlayerConfig.h"
#include "GameInputSubsystem.h"

AMyGameStateBase::AMyGameStateBase() {
	TotalNumberOfCheckpoints = 0;
	bResetTrack = false;
	bReceivedCheckpoints = false;
	bReceivedStartingBlocks = false;
	bPreviousCheckpointColor = false;
	LocalQuadcopter = nullptr;
	LocalPlayerState = nullptr;
	LocalHud = nullptr;
	bFinishedPreRace = false;
}
void AMyGameStateBase::BeginPlay() {
	Super::BeginPlay();
	GetWorld()->OnWorldBeginPlay.AddUObject(this, &AMyGameStateBase::PostBeginPlay);
}
void AMyGameStateBase::PostBeginPlay() {
	for (TActorIterator<APlayerController> i(GetWorld()); i; ++i) {
		if (i->IsLocalPlayerController()) {
			i->ConsoleCommand("t.maxfps 164");
			LocalPlayerState = Cast<AMyPlayerState>(i->PlayerState);
			LocalQuadcopter = Cast<AQuadcopter>(i->GetPawn());
			LocalHud = i->GetHUD<AMyHUD>();
			LocalHud->EnableInput(*i);
			LocalHud->CreateHud();
			LocalHud->SetActorTickEnabled(true);
			UPlayerConfig::LoadPlayerConfig(*LocalQuadcopter, **i);
			GEngine->GetEngineSubsystem<UGameInputSubsystem>()->LoadControllerConfig();
			bFinishedPreRace = true;
			OnRep_bResetTrack();
			break;
		}
	}
}
void AMyGameStateBase::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	for (TActorIterator<APlayerController> i(GetWorld()); i; ++i) {
		if (i->IsLocalPlayerController()) {
			i->ConsoleCommand("t.maxfps 30");
		}
	}
	Super::EndPlay(EndPlayReason);
}
void AMyGameStateBase::OnCheckpointTriggered(const FCheckpointEvent& CheckpointEvent) {
	auto Checkpoint = Cast<ACheckpoint>(CheckpointEvent.Hit.Actor);
	auto CheckpointIndex = Checkpoint->GetCheckpointIndex();
	if (CheckpointIndex == LocalPlayerState->CurrentCheckpointIndex) {
		if (FVector::DotProduct(CurrentCheckpointRay.Direction, LocalQuadcopter->GetVelocity().GetSafeNormal()) > 0.f) {
			auto NextCheckpointIndex = (LocalPlayerState->NextCheckpointIndex == (TotalNumberOfCheckpoints - 1)) ? 0 : (LocalPlayerState->NextCheckpointIndex + 1);
			LocalPlayerState->OnCheckpointTriggered(NextCheckpointIndex);
			Checkpoints[LocalPlayerState->CurrentCheckpointIndex]->GetCheckpointRay(CurrentCheckpointRay);
			FRay NextCheckpointRay;
			Checkpoints[LocalPlayerState->NextCheckpointIndex]->GetCheckpointRay(NextCheckpointRay);
			LocalHud->OnCheckpointTriggered(NextCheckpointRay);
		}
	}	
}

void AMyGameStateBase::RestartRun() {
	// set starting location and rotation
	auto InitialLocation = FVector::ZeroVector;
	auto InitialRotation = FRotator::ZeroRotator;
	if (StartingBlocks.IsValidIndex(0)) {
		InitialLocation = StartingBlocks[0]->GetActorLocation();
		InitialRotation = StartingBlocks[0]->GetActorRotation();
	}
	// move quadcopter and stop physics
	LocalQuadcopter->SetActorLocationAndRotation(InitialLocation, InitialRotation);
	LocalQuadcopter->StopAllPhysics();
	// initialize player state
	LocalPlayerState->OnRestartRun();
	Checkpoints[LocalPlayerState->CurrentCheckpointIndex]->GetCheckpointRay(CurrentCheckpointRay);
	Checkpoints[LocalPlayerState->CurrentCheckpointIndex]->GetCheckpointRay(LocalHud->CurrentCheckpointRay);
	Checkpoints[LocalPlayerState->NextCheckpointIndex]->GetCheckpointRay(LocalHud->NextCheckpointRay);
}
void AMyGameStateBase::OnRep_bResetTrack() {
	if (bResetTrack && bReceivedCheckpoints && bReceivedStartingBlocks && bFinishedPreRace) {
		TotalNumberOfCheckpoints = Checkpoints.Num();
		if (TotalNumberOfCheckpoints > 1) {
			LocalQuadcopter->WantsRestartRun.AddDynamic(this, &AMyGameStateBase::RestartRun);
			LocalQuadcopter->CheckpointTriggered.AddDynamic(this, &AMyGameStateBase::OnCheckpointTriggered);
			RestartRun();
			LocalHud->bShowCheckpointMarkers = true;
		}
	}
}
void AMyGameStateBase::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams SharedParams;

	SharedParams.bIsPushBased = true;
	DOREPLIFETIME_WITH_PARAMS_FAST(AMyGameStateBase, Checkpoints, SharedParams);
	DOREPLIFETIME_WITH_PARAMS_FAST(AMyGameStateBase, StartingBlocks, SharedParams);
	SharedParams.bIsPushBased = false;
	DOREPLIFETIME_WITH_PARAMS_FAST(AMyGameStateBase, bResetTrack, SharedParams);

}
void AMyGameStateBase::OnRep_Checkpoints() {
	bReceivedCheckpoints = true;
	OnRep_bResetTrack();
}
void AMyGameStateBase::OnRep_StartingBlocks() {
	bReceivedStartingBlocks = true;
	OnRep_bResetTrack();
}



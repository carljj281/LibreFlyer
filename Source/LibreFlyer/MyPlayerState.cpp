// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"
#include "Kismet/GameplayStatics.h"

AMyPlayerState::AMyPlayerState() {
	CurrentCheckpointIndex = -1;
	NextCheckpointIndex = -1;
	CheckpointSound = nullptr;
	PersonalBest = 0.f;
	bHasName = false;

	CheckpointSound = LoadObject<USoundBase>(
		nullptr,
		TEXT("SoundWave'/Game/CheckpointSound.CheckpointSound'")
	);
}
void AMyPlayerState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams SharedParams;
	SharedParams.bIsPushBased = false;
	DOREPLIFETIME_WITH_PARAMS_FAST(AMyPlayerState, CheckpointSplits, SharedParams);
	DOREPLIFETIME_WITH_PARAMS_FAST(AMyPlayerState, PersonalBest, SharedParams);
	DOREPLIFETIME_WITH_PARAMS_FAST(AMyPlayerState, PersonalBestSplits, SharedParams);
}
void AMyPlayerState::SetPlayerNameInternal(const FString& S) {
	Super::SetPlayerNameInternal(S);
	bHasName = true;
}
void AMyPlayerState::OnRestartRun() {
	CurrentCheckpointIndex = 0;
	NextCheckpointIndex = 1;
}
void AMyPlayerState::OnCheckpointTriggered(int NewCheckpointIndex) {
	auto SplitTime = 0.f;
	auto SplitDifference = 0.f;
	// first checkpoint
	if (CurrentCheckpointIndex == 0) {
		// todo: dont use FPlatformTime (this is real-world time) because higher framerates will trigger checkpoints slightly sooner than low framerates
		// do research on how to make checkpoints trigger the same for all framerates
		// maybe set a fixed tickrate or something idk
		RunStartTime = FPlatformTime::Seconds();
	}
	else {
		SplitTime = FPlatformTime::Seconds() - RunStartTime;
		// update playerstate
		if (CheckpointSplits.IsValidIndex(CurrentCheckpointIndex - 1)) {
			CheckpointSplits[CurrentCheckpointIndex - 1] = SplitTime;
		}
		else {
			CheckpointSplits.Add(SplitTime);
		}
		// last checkpoint
		if (NewCheckpointIndex == 0) {
			if( PersonalBest == 0.f || SplitTime < PersonalBest) {
				PersonalBest = SplitTime;
				PersonalBestSplits = CheckpointSplits;
			}
		}
	}
	// play sound
	UGameplayStatics::PlaySound2D(this, CheckpointSound);
	// increment indexes
	CurrentCheckpointIndex = NextCheckpointIndex;
	NextCheckpointIndex = NewCheckpointIndex;
}

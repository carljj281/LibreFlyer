// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "MyPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class LIBREFLYER_API AMyPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	AMyPlayerState();
	int CurrentCheckpointIndex;
	int NextCheckpointIndex;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* CheckpointSound;

	double RunStartTime;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = PlayerState)
	TArray<float> CheckpointSplits;
	// todo : use the last element of PersonalBestSplits and get rid of PersonalBest
	UPROPERTY(Replicated, BlueprintReadOnly, Category = PlayerState)
	float PersonalBest;
	UPROPERTY(Replicated, BlueprintReadOnly, Category = PlayerState)
	TArray<float> PersonalBestSplits;


	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
	virtual void SetPlayerNameInternal(const FString& S) override;
	bool bHasName;

	void OnRestartRun();
	void OnCheckpointTriggered(int NewCheckpointIndex);
};

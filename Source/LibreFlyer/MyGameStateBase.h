// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "LibreFlyerStuff.h"
#include "MyGameStateBase.generated.h"

class AQuadcopter;
class AMyPlayerState;
class AMyHUD;
class ACheckpoint;
class AMyGameModeBase;

UCLASS()
class LIBREFLYER_API AMyGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
public:
	friend AMyGameModeBase;

	AMyGameStateBase();
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AQuadcopter* LocalQuadcopter;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AMyPlayerState* LocalPlayerState;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AMyHUD* LocalHud;
	UPROPERTY(ReplicatedUsing = OnRep_Checkpoints, Replicated, EditAnywhere, BlueprintReadWrite)
	TArray<ACheckpoint*> Checkpoints;
	UPROPERTY(ReplicatedUsing = OnRep_StartingBlocks, Replicated, EditAnywhere, BlueprintReadWrite)
	TArray<AActor*> StartingBlocks;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int TotalNumberOfCheckpoints;
	UFUNCTION(BlueprintCallable)
	void RestartRun();
	void PostBeginPlay();

protected:

	UFUNCTION()
	virtual void OnRep_bResetTrack();
	UFUNCTION()
	virtual void OnRep_Checkpoints();
	UFUNCTION()
	virtual void OnRep_StartingBlocks();

	UPROPERTY(ReplicatedUsing = OnRep_bResetTrack, BlueprintReadOnly)
	bool bResetTrack;
	bool bReceivedCheckpoints;
	bool bReceivedStartingBlocks;
	bool bFinishedPreRace;
	uint8 bPreviousCheckpointColor : 1;
	FRay CurrentCheckpointRay;
	UFUNCTION()
	void OnCheckpointTriggered(const FCheckpointEvent& CheckpointEvent);

};

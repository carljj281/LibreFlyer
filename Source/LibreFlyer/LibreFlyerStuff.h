#pragma once

//#include "LibreFlyerStuff.h"

#include "LibreFlyerStuff.generated.h"

static const float FloatZero = 0.f;
static FString LibreConfigPath = FPaths::ProjectConfigDir() / TEXT("LibreConfigs");


USTRUCT(BlueprintType)
struct FCheckpointEvent {
	GENERATED_BODY()
	FHitResult Hit;

	FCheckpointEvent() {
		Hit = FHitResult();
	}
	FCheckpointEvent(FHitResult H) {
		Hit = H;
	}
};


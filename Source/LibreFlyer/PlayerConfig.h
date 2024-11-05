// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PlayerConfig.generated.h"

class AQuadcopter;

UCLASS()
class LIBREFLYER_API UPlayerConfig : public UObject {
	GENERATED_BODY()
public:
	static void LoadPlayerConfig(AQuadcopter& LocalQuadcopter, APlayerController& LocalPlayerController);
};

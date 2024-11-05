// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "GameInputSubsystemStuff.h"
#include "LibreFlyerStuff.h"

#include "GameInputSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInputDeviceConnect, const FGameInputDevice&, GameInputDevice);


UCLASS()
class LIBREFLYER_API UGameInputSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()
public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintPure)
	void GetQuadcopterInputs(
		float& ThrottleInput, 
		float& RollInput,
		float& PitchInput, 
		float& YawInput
	);

	UFUNCTION(BlueprintPure)
	void GetAxisInput(EAxisInputType InputTypeIndex, float& Output);

	UFUNCTION(BlueprintPure)
	bool GetAxisInputByIndex(const int& AxisIndex, float& Output);

	UPROPERTY(BlueprintAssignable)
	FInputDeviceConnect OnInputDeviceConnect;

	IGameInput* GameInput;
	GameInputCallbackToken DeviceCallbackToken;
	GameInputCallbackToken ReadingCallbackToken;

	IGameInputReading*  PreviousReading;
	IGameInputReading*  CurrentReading;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FGameInputDevice> GameInputDevices;

	static void CALLBACK DeviceCallback(
		_In_ GameInputCallbackToken callbackToken,
		_In_ void* context,
		_In_ IGameInputDevice* device,
		_In_ uint64_t timestamp,
		_In_ GameInputDeviceStatus currentStatus,
		_In_ GameInputDeviceStatus previousStatus) noexcept;

	void LoadControllerConfig(int ControllerIndex = -1);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int CurrentControllerIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHasDevice;
};

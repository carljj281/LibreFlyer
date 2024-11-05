// Fill out your copyright notice in the Description page of Project Settings.


#include "GameInputSubsystem.h"
#include "controlrate_profile.h"

void UGameInputSubsystem::Initialize(FSubsystemCollectionBase& Collection) {
	bHasDevice = false;
	CurrentControllerIndex = 0;
	GameInput = nullptr;

	GameInputCreate(&GameInput);

	GameInput->RegisterDeviceCallback(
		nullptr,
		AllGameInputKinds,
		GameInputDeviceAnyStatus,
		GameInputAsyncEnumeration,
		this,
		&DeviceCallback,
		&DeviceCallbackToken);	
}
void UGameInputSubsystem::Deinitialize() {
	GameInput->UnregisterCallback(DeviceCallbackToken, UINT64_MAX);
}
void UGameInputSubsystem::GetQuadcopterInputs(float& ThrottleInput, float& RollInput, float& PitchInput, float& YawInput) {
	if (bHasDevice) {
		auto Result = GameInput->GetCurrentReading(GameInputKindController, GameInputDevices[CurrentControllerIndex].ActualDevice, &CurrentReading);
		if (SUCCEEDED(Result)) {
			CurrentReading->GetControllerAxisState(
				GameInputDevices[CurrentControllerIndex].AxisCount,
				GameInputDevices[CurrentControllerIndex].AxisReadings.GetData());
			PreviousReading = CurrentReading;
			CurrentReading->Release();
			GetAxisInput(EAxisInputType::Throttle, ThrottleInput);
			GetAxisInput(EAxisInputType::Roll, RollInput);
			GetAxisInput(EAxisInputType::Pitch, PitchInput);
			GetAxisInput(EAxisInputType::Yaw, YawInput);
			return;
		}
	}
}
void UGameInputSubsystem::GetAxisInput(EAxisInputType InputType, float& Output) {
	//todo - ##TICKIMPROVEMENTS this whole input pipeline is just a rough draft and can be overhauled. Get rid of index validations, prefetch, etc.
	// map input type to axis index instead of using IndexOfByKey()
	auto AxisIndex = GameInputDevices[CurrentControllerIndex].AxisProperties.IndexOfByKey(InputType);
	if (GetAxisInputByIndex(AxisIndex, Output)) {
		if (InputType == EAxisInputType::Throttle) {
			if (GameInputDevices[CurrentControllerIndex].bMidStickThrottle) {
				Output *= 0.5f;
				Output += 0.5f;
			}
			Output = fmaxf(Output, 0.f);
		}
		else {
			float AbsoluteInput = fabsf(Output);
			if (AbsoluteInput > (GameInputDevices[CurrentControllerIndex].AxisProperties[AxisIndex].Deadzone)) {
				Output = copysignf(1.f, Output) *
					(
						(AbsoluteInput - GameInputDevices[CurrentControllerIndex].AxisProperties[AxisIndex].Deadzone) /
						(1.f - (GameInputDevices[CurrentControllerIndex].AxisProperties[AxisIndex].Deadzone))
						);
				switch (currentControlRateProfile.rates_type) {
				case RATES_TYPE_BETAFLIGHT:
					Output = applyBetaflightRates((int)InputType - 2, Output, AbsoluteInput);
					break;
				case RATES_TYPE_RACEFLIGHT:
					Output = applyRaceFlightRates((int)InputType - 2, Output, AbsoluteInput);
					break;
				case RATES_TYPE_KISS:
					Output = applyKissRates((int)InputType - 2, Output, AbsoluteInput);
					break;
				case RATES_TYPE_ACTUAL:
					Output = applyActualRates((int)InputType - 2, Output, AbsoluteInput);
					break;
				case RATES_TYPE_QUICK:
					Output = applyQuickRates((int)InputType - 2, Output, AbsoluteInput);
					break;
				case RATES_TYPE_EASY:
					Output *= EasyRates.MaxRotationSpeed *
						(
							EasyRates.CenterStickPrecision *
							powf(AbsoluteInput, 50.f * powf(EasyRates.OuterStickTransition, 7.f)) +
							(1.f - EasyRates.CenterStickPrecision)
							);
					break;
				}
			}
		}
	}
}
bool UGameInputSubsystem::GetAxisInputByIndex(const int& AxisIndex, float& Output) {
	if (GameInputDevices[CurrentControllerIndex].AxisReadings.IsValidIndex(AxisIndex)) {

		Output = GameInputDevices[CurrentControllerIndex].AxisReadings[AxisIndex];
		Output += GameInputDevices[CurrentControllerIndex].AxisProperties[AxisIndex].Offset;
		Output *= GameInputDevices[CurrentControllerIndex].AxisProperties[AxisIndex].Scale;
		if (GameInputDevices[CurrentControllerIndex].AxisProperties[AxisIndex].Invert) {
			Output *= -1.f;
		}
		return true;
	}
	return false;
}
void UGameInputSubsystem::DeviceCallback(GameInputCallbackToken callbackToken, void* context, IGameInputDevice* device, uint64_t timestamp, GameInputDeviceStatus currentStatus, GameInputDeviceStatus previousStatus) noexcept {
	auto GameInputSubsystem = static_cast<UGameInputSubsystem*>(context);
	if (device->GetDeviceInfo()->supportedInput & GameInputKindController) {
		FGameInputDevice NewDevice(*device);
		GameInputSubsystem->GameInputDevices.Add(NewDevice);
		GameInputSubsystem->bHasDevice = true;
	}
}
void UGameInputSubsystem::LoadControllerConfig(int ControllerIndex) {
	if (ControllerIndex != -1) CurrentControllerIndex = ControllerIndex;
	if (!GameInputDevices.IsValidIndex(CurrentControllerIndex)) return;
	auto& GameInputDevice = GameInputDevices[CurrentControllerIndex];
	auto ConfigFilePath = LibreConfigPath / "Controllers" / LexToString(GameInputDevice.VendorId) + LexToString(GameInputDevice.ProductId) + TEXT(".txt");
	TArray<FString> ConfigLines;
	if (!FPaths::FileExists(ConfigFilePath)) {
		ConfigLines.Add(FString::Printf(TEXT("%s"), *FString("-------------------- Input type : 0 = None 1 = Throttle, 2 = Roll, 3 = Pitch, 4 = Yaw --------------------")));
		for (size_t i = 0; i < (GameInputDevice.AxisCount); i++) {
			ConfigLines.Add(FString::Printf(TEXT("Axis %d :"), i));
			ConfigLines.Add(TEXT("     ") + FString::Printf(TEXT("Input type = %d"), GameInputDevice.AxisProperties[i].InputType));
			ConfigLines.Add(TEXT("     ") + FString::Printf(TEXT("Invert = %s"), *LexToString(GameInputDevice.AxisProperties[i].Invert)));

			ConfigLines.Add(TEXT("     ") + FString::Printf(TEXT("Offset = %f"), GameInputDevice.AxisProperties[i].Offset));
			ConfigLines.Add(TEXT("     ") + FString::Printf(TEXT("Scale = %f"), GameInputDevice.AxisProperties[i].Scale));

			ConfigLines.Add(TEXT("     ") + FString::Printf(TEXT("Deadzone = %f"), *LexToSanitizedString(GameInputDevice.AxisProperties[i].Deadzone)));
			ConfigLines.Add(TEXT("     ") + FString::Printf(TEXT("Min = %f"), GameInputDevice.AxisProperties[i].Min));
			ConfigLines.Add(TEXT("     ") + FString::Printf(TEXT("Max = %f"), GameInputDevice.AxisProperties[i].Max));
		}
		FFileHelper::SaveStringArrayToFile(ConfigLines, *ConfigFilePath);
	}
	else {
		FFileHelper::LoadFileToStringArray(ConfigLines, *ConfigFilePath);
	}
	for (size_t i = 0; i < ConfigLines.Num(); i++) {
		if (ConfigLines[i].Contains("Axis")) {
			ConfigLines[i].RemoveSpacesInline();
			ConfigLines[i].RemoveFromStart("Axis");
			ConfigLines[i].RemoveFromEnd(":");
			int AxisNumber = FCString::Atoi(*ConfigLines[i]);

			for (size_t i2 = 1; i2 < 7; i2++) {
				if (!ConfigLines[i + i2].Contains("=")) continue;
				ConfigLines[i + i2].RemoveSpacesInline();
				ConfigLines[i + i2].ToLowerInline();
				auto EqualSignIndex = ConfigLines[i + i2].Find("=") + 1;
				auto ValueString = ConfigLines[i + i2].RightChop(EqualSignIndex);
				ConfigLines[i + i2].LeftInline(EqualSignIndex);

				if (ConfigLines[i + i2].Equals("inputtype=")) {
					GameInputDevice.AxisProperties[AxisNumber].InputType = static_cast<EAxisInputType>(FCString::Atoi(*ValueString));
				}
				else if (ConfigLines[i + i2].Equals("invert=")) {
					GameInputDevice.AxisProperties[AxisNumber].Invert = FCString::ToBool(*ValueString);
				}
				else if (ConfigLines[i + i2].Equals("offset=")) {
					GameInputDevice.AxisProperties[AxisNumber].Offset = FCString::Atof(*ValueString);
				}
				else if (ConfigLines[i + i2].Equals("scale=")) {
					GameInputDevice.AxisProperties[AxisNumber].Scale = FCString::Atof(*ValueString);
				}
				else if (ConfigLines[i + i2].Equals("deadzone=")) {
					GameInputDevice.AxisProperties[AxisNumber].Deadzone = FCString::Atof(*ValueString);
				}
				else if (ConfigLines[i + i2].Equals("min=")) {
					GameInputDevice.AxisProperties[AxisNumber].Min = FCString::Atof(*ValueString);
				}
				else if (ConfigLines[i + i2].Equals("max=")) {
					GameInputDevice.AxisProperties[AxisNumber].Max = FCString::Atof(*ValueString);
				}
			}
		}
	}
}

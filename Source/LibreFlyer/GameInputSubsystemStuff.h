#pragma once

#include "Windows/WindowsApplication.h"
#include "GameInput.h"
#include "GameInputSubsystemStuff.generated.h"

UENUM(BlueprintType)
enum class EAxisInputType : uint8 {
	None,
	Throttle,
	Roll,
	Pitch,
	Yaw
};
USTRUCT(BlueprintType)
struct FGameInputAxisProperties {
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAxisInputType InputType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Invert;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Offset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Scale;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Deadzone;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Min;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Max;
	FGameInputAxisProperties(){
		InputType = EAxisInputType::None;
		Invert = false;
		Scale = 1.f;
		Offset = 0.f;
		Deadzone = 0.f;
		Min = -1.f;
		Max = 1.f;
	}
	bool operator==(const EAxisInputType& CompareToThis) const {
		return InputType == CompareToThis;
	}
};
USTRUCT(BlueprintType)
struct FGameInputDevice {
	GENERATED_BODY()
	FGameInputDevice() {};
	FGameInputDevice(IGameInputDevice& device) {
		ActualDevice = &device;
		VendorId = device.GetDeviceInfo()->vendorId;
		ProductId = device.GetDeviceInfo()->productId;
		RevisionNumber = device.GetDeviceInfo()->revisionNumber;
		AxisCount = device.GetDeviceInfo()->controllerAxisCount;
		AxisReadings.Init(0.f, AxisCount);
		ButtonCount = device.GetDeviceInfo()->controllerButtonCount;
		ButtonStates.Init(0.f, ButtonCount);
		SwitchCount = device.GetDeviceInfo()->controllerSwitchCount;
		SwitchStates.Init(GameInputSwitchCenter, SwitchCount);
		DeviceStatus = device.GetDeviceStatus();

		AxisProperties.Init(FGameInputAxisProperties(), AxisCount);
		bMidStickThrottle = false;
	}
	IGameInputDevice* ActualDevice;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int VendorId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int ProductId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int RevisionNumber;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<float> AxisReadings;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int AxisCount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<bool> ButtonStates;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int ButtonCount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<uint8> SwitchStates;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int SwitchCount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int DeviceStatus;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FGameInputAxisProperties> AxisProperties;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bMidStickThrottle;
};
static GameInputKind AllGameInputKinds =
GameInputKindUnknown |
GameInputKindRawDeviceReport |
GameInputKindControllerAxis |
GameInputKindControllerButton |
GameInputKindControllerSwitch |
GameInputKindController |
GameInputKindKeyboard |
GameInputKindMouse |
GameInputKindTouch |
GameInputKindMotion |
GameInputKindArcadeStick |
GameInputKindFlightStick |
GameInputKindGamepad |
GameInputKindRacingWheel |
GameInputKindUiNavigation;

static FString LexToString(GameInputDeviceStatus DeviceStatus) {
	if (DeviceStatus == GameInputDeviceNoStatus) {
		return TEXT("NoStatus");
	}
	else if (DeviceStatus == GameInputDeviceAnyStatus) {
		return TEXT("Any");
	}

	FString Result = TEXT("");
#define DEVICE_STATUS(StatusFlag,DisplayName) if( DeviceStatus & StatusFlag ) Result += (FString(DisplayName) + TEXT("|"));
	DEVICE_STATUS(GameInputDeviceConnected, TEXT("Connected"));
	DEVICE_STATUS(GameInputDeviceInputEnabled, TEXT("InputEnabled"));
	DEVICE_STATUS(GameInputDeviceOutputEnabled, TEXT("OutputEnabled"));
	DEVICE_STATUS(GameInputDeviceRawIoEnabled, TEXT("RawIoEnabled"));
	DEVICE_STATUS(GameInputDeviceAudioCapture, TEXT("AudioCapture"));
	DEVICE_STATUS(GameInputDeviceAudioRender, TEXT("AudioRender"));
	DEVICE_STATUS(GameInputDeviceSynchronized, TEXT("Synchronized"));
	DEVICE_STATUS(GameInputDeviceWireless, TEXT("Wireless"));
	DEVICE_STATUS(GameInputDeviceUserIdle, TEXT("UserIdle"));
#undef DEVICE_STATUS

	Result.RemoveFromEnd(TEXT("|"));
	return Result;
}

static FString LexToString(GameInputKind InputKind) {
	if (InputKind == GameInputKindUnknown) {
		return TEXT("Unknown");
	}

	FString Result = TEXT("");
#define INPUT_KIND_STRING(StatusFlag, DisplayName) if( InputKind & StatusFlag ) Result += (FString(DisplayName) + TEXT("|"));
	INPUT_KIND_STRING(GameInputKindRawDeviceReport, TEXT("RawDeviceReport"));
	INPUT_KIND_STRING(GameInputKindControllerAxis, TEXT("ControllerAxis"));
	INPUT_KIND_STRING(GameInputKindControllerButton, TEXT("ControllerButton"));
	INPUT_KIND_STRING(GameInputKindControllerSwitch, TEXT("ControllerSwitch"));
	INPUT_KIND_STRING(GameInputKindController, TEXT("Controller"));
	INPUT_KIND_STRING(GameInputKindKeyboard, TEXT("Keyboard"));
	INPUT_KIND_STRING(GameInputKindMouse, TEXT("Mouse"));
	INPUT_KIND_STRING(GameInputKindTouch, TEXT("Touch"));
	INPUT_KIND_STRING(GameInputKindMotion, TEXT("Motion"));
	INPUT_KIND_STRING(GameInputKindArcadeStick, TEXT("ArcadeStick"));
	INPUT_KIND_STRING(GameInputKindFlightStick, TEXT("FlightStick"));
	INPUT_KIND_STRING(GameInputKindGamepad, TEXT("Gamepad"));
	INPUT_KIND_STRING(GameInputKindRacingWheel, TEXT("RacingWheel"));
	INPUT_KIND_STRING(GameInputKindUiNavigation, TEXT("UiNavigation"));
#undef INPUT_KIND_STRING

	Result.RemoveFromEnd(TEXT("|"));
	return Result;
}

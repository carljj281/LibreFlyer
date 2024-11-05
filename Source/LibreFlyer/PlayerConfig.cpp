// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerConfig.h"
#include "Quadcopter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/InputSettings.h"
#include "GameFramework/PlayerController.h"
#include "Misc/DefaultValueHelper.h"
#include "LibreFlyerStuff.h"
#include "controlrate_profile.h"

void UPlayerConfig::LoadPlayerConfig(AQuadcopter& LocalQuadcopter, APlayerController& LocalPlayerController) {
	FString PlayerName = TEXT("DefaultPlayer");
	auto ConfigFilePath = LibreConfigPath / PlayerName + TEXT(".txt");
	TArray<FString> ConfigLines;
	if (!FPaths::FileExists(ConfigFilePath)) {
		ConfigLines.Add(FString::Printf(TEXT("%s = %f"), *FString("Field of View"), 130.0));
		ConfigLines.Add(FString::Printf(TEXT("%s = %f"), *FString("Camera Angle"), 55.0));
		ConfigLines.Add("");
		ConfigLines.Add(FString::Printf(TEXT("%s"), *FString("-------------------- 0 = Betaflight, 1 = Raceflight, 2 = Kiss, 3 = Actual, 4 = Quick, 5 = Easy --------------------")));
		ConfigLines.Add(FString::Printf(TEXT("%s = %i"), *FString("Rates Type"), 0));
		ConfigLines.Add(FString::Printf(TEXT("%s"), *FString("-------------------- Roll, Pitch, Yaw--------------------")));
		ConfigLines.Add(FString::Printf(TEXT("%s = %f, %f, %f"), *FString("RC Rate"), 1.0, 1.0, 1.0));
		ConfigLines.Add(FString::Printf(TEXT("%s = %f, %f, %f"), *FString("Super Rate"), 0.7, 0.7, 0.7));
		ConfigLines.Add(FString::Printf(TEXT("%s = %f, %f, %f"), *FString("RC Expo"), 0.0, 0.0, 0.0));
		ConfigLines.Add(FString::Printf(TEXT("%s = %f"), *FString("Thottle Mid"), 0.5));
		ConfigLines.Add(FString::Printf(TEXT("%s = %f"), *FString("Throttle Expo"), 0.0));
		//ConfigLines.Add("");
		//ConfigLines.Add(FString::Printf(TEXT("%s = %s"), *FString("Restart Run"), *FString("GenericUSBController_Button5")));
		//ConfigLines.Add(FString::Printf(TEXT("%s = %s"), *FString("-Throttle"), *FString("GenericUSBController_Axis3")));
		//ConfigLines.Add(FString::Printf(TEXT("%s = %s"), *FString("Pitch"), *FString("GenericUSBController_Axis1")));
		//ConfigLines.Add(FString::Printf(TEXT("%s = %s"), *FString("Yaw"), *FString("GenericUSBController_Axis4")));
		//ConfigLines.Add(FString::Printf(TEXT("%s = %s"), *FString("Roll"), *FString("GenericUSBController_Axis2")));
		ConfigLines.Add("");
		ConfigLines.Add(FString::Printf(TEXT("%s = %s"), *FString("MousePitch"), *FString("MouseY")));
		ConfigLines.Add(FString::Printf(TEXT("%s = %s"), *FString("MouseRoll"), *FString("MouseX")));
		ConfigLines.Add(FString::Printf(TEXT("%s = %f"), *FString("Mouse Sensitivity"), 0.15));
		ConfigLines.Add(FString::Printf(TEXT("%s = %s"), *FString("Escape Menu"), *FString("Escape")));
		ConfigLines.Add(FString::Printf(TEXT("%s = %s"), *FString("Restart Run"), *FString("SpaceBar")));
		ConfigLines.Add(FString::Printf(TEXT("%s = %s"), *FString("Throttle"), *FString("W")));
		ConfigLines.Add(FString::Printf(TEXT("%s = %s"), *FString("KeyboardYaw"), *FString("D")));
		ConfigLines.Add(FString::Printf(TEXT("%s = %s"), *FString("-KeyboardYaw"), *FString("A")));

		FFileHelper::SaveStringArrayToFile(ConfigLines, *ConfigFilePath);
		ConfigLines.Empty();
	}
	FFileHelper::LoadFileToStringArray(ConfigLines, *ConfigFilePath);
	for (auto& i : ConfigLines) {
		if (!i.Contains("=")) continue;
		i.RemoveSpacesInline();
		i.ToLowerInline();
		auto EqualSignIndex = i.Find("=") + 1;
		auto ValueString = i.RightChop(EqualSignIndex);
		i.LeftInline(EqualSignIndex);
		auto bIsNegative = i.StartsWith("-");
		if (bIsNegative) {
			i.RemoveAt(0);
		}
		else {
			if (i.Equals("fieldofview=")) {
				LocalQuadcopter.QuadcopterCamera->FieldOfView = FCString::Atof(*ValueString);
			}
			else if (i.Equals("cameraangle=")) {
				LocalQuadcopter.QuadcopterCamera->SetRelativeRotation(FRotator(FCString::Atof(*ValueString), 0.f, 0.f));
			}
			else if (i.Equals("ratestype=")) {
				currentControlRateProfile.rates_type = FCString::Atoi(*ValueString);
			}
			else if (i.Equals("rcrate=")) {
				TArray<FString> CurrentValues;
				ValueString.ParseIntoArray(CurrentValues, TEXT(","));
				for (size_t i2 = 0; i2 < 3; i2++) {
					currentControlRateProfile.rcRates[i2] = FCString::Atof(*CurrentValues[i2]) * 100.f;
				}
			}
			else if (i.Equals("superrate=")) {
				TArray<FString> CurrentValues;
				ValueString.ParseIntoArray(CurrentValues, TEXT(","));
				for (size_t i2 = 0; i2 < 3; i2++) {
					currentControlRateProfile.rates[i2] = FCString::Atof(*CurrentValues[i2]) * 100.f;
				}
			}
			else if (i.Equals("rcexpo=")) {
				TArray<FString> CurrentValues;
				ValueString.ParseIntoArray(CurrentValues, TEXT(","));
				for (size_t i2 = 0; i2 < 3; i2++) {
					currentControlRateProfile.rcExpo[i2] = FCString::Atof(*CurrentValues[i2]) * 100.f;
				}
			}
			else if (i.Equals("throttlemid=")) {
				currentControlRateProfile.thrMid8 = FCString::Atof(*ValueString);
			}
			else if (i.Equals("throttleexpo=")) {
				currentControlRateProfile.thrExpo8 = FCString::Atof(*ValueString);
			}
			else if (i.Equals("restartrun=")) {
				LocalPlayerController.PlayerInput->AddActionMapping(FInputActionKeyMapping("RestartRun", *ValueString));
			}
			else if (i.Equals("escapemenu=")) {
				LocalPlayerController.PlayerInput->AddActionMapping(FInputActionKeyMapping("EscapeMenu", *ValueString));
			}
		}
		if (i.Equals("throttle=")) {
			LocalPlayerController.PlayerInput->AddAxisMapping(FInputAxisKeyMapping("Throttle", *ValueString, bIsNegative ? -1.f : 1.f));
		}
		else if (i.Equals("keyboardpitch=")) {
			LocalPlayerController.PlayerInput->AddAxisMapping(FInputAxisKeyMapping("KeyboardPitch", *ValueString, bIsNegative ? -1.f : 1.f));
		}
		else if (i.Equals("keyboardyaw=")) {
			LocalPlayerController.PlayerInput->AddAxisMapping(FInputAxisKeyMapping("KeyboardYaw", *ValueString, bIsNegative ? -1.f : 1.f));
		}
		else if (i.Equals("keyboardroll=")) {
			LocalPlayerController.PlayerInput->AddAxisMapping(FInputAxisKeyMapping("KeyboardRoll", *ValueString, bIsNegative ? -1.f : 1.f));
		}
		else if (i.Equals("mousepitch=")) {
			LocalPlayerController.PlayerInput->AddAxisMapping(FInputAxisKeyMapping("MousePitch", *ValueString, bIsNegative ? -1.f : 1.f));
		}
		else if (i.Equals("mouseyaw=")) {
			LocalPlayerController.PlayerInput->AddAxisMapping(FInputAxisKeyMapping("MouseYaw", *ValueString, bIsNegative ? -1.f : 1.f));
		}
		else if (i.Equals("mouseroll=")) {
			LocalPlayerController.PlayerInput->AddAxisMapping(FInputAxisKeyMapping("MouseRoll", *ValueString, bIsNegative ? -1.f : 1.f));
		}
	}	
}

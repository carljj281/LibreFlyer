 //Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "controlrate_profile.h"
#include "LibreFlyerStuff.h"
#include "Quadcopter.generated.h"

class UCameraComponent;
class UBoxComponent;
class USphereComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWantsRestartRun);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCheckpointTriggered, const FCheckpointEvent&, CheckpointEvent);

USTRUCT(BlueprintType)
struct FGamepadProperties {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxRotationSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CenterStickPrecision;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float OuterStickTransition;

	FGamepadProperties() {
		MaxRotationSpeed = 475.f;
		CenterStickPrecision = 0.6f;
		OuterStickTransition = 0.3f;
	}
};

UCLASS()
class LIBREFLYER_API AQuadcopter : public APawn {
	GENERATED_BODY()
public:
	AQuadcopter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void Throttle(float Input);
	void KeyboardYaw(float Input);
	void MousePitch(float Input);
	void MouseRoll(float Input);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCameraComponent* QuadcopterCamera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* QuadcopterRoot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* QuadcopterModel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGamepadProperties GamepadProperties;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator RotationInput;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator RelativeInput;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHasRelativeRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ThrottleInput;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ThrusterForce;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ThrusterDistance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ThrusterOffset;

	double MouseSensitivity;
	void SetMouseSensitivity(double MouseDpi, double CentimetersPer360);

	void RestartRun();
	FVector InitialLocation;
	FRotator InitialRotation;

	UPROPERTY(BlueprintAssignable)
	FWantsRestartRun	WantsRestartRun;

	UFUNCTION(BlueprintCallable)
	void RunDebugFunction();

	FVector PreviousLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ThrottleExponent;

	void StopAllPhysics();

	UPROPERTY(BlueprintAssignable)
	FCheckpointTriggered CheckpointTriggered;

	FCollisionQueryParams HitParameters;
};

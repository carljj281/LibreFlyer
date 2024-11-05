// Fill out your copyright notice in the Description page of Project Settings.


#include "Quadcopter.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "GameInputSubsystem.h"

AQuadcopter::AQuadcopter() {
	PrimaryActorTick.bCanEverTick = true;
	//PrimaryActorTick.bStartWithTickEnabled = false;

	QuadcopterRoot = CreateDefaultSubobject<UStaticMeshComponent>("QuadcopterRoot");
	QuadcopterRoot->SetCollisionProfileName("BlockAllDynamic");
	auto PhysicsMaterialOverride = NewObject<UPhysicalMaterial>();
	PhysicsMaterialOverride->Friction = 0.f;
	PhysicsMaterialOverride->StaticFriction = 0.f;
	PhysicsMaterialOverride->Restitution = 0.f;
	PhysicsMaterialOverride->RaiseMassToPower = 1.f;
	QuadcopterRoot->SetPhysMaterialOverride(PhysicsMaterialOverride);
	QuadcopterRoot->SetSimulatePhysics(true);
	QuadcopterRoot->SetLinearDamping(1.0f);
	QuadcopterRoot->SetMassOverrideInKg(NAME_None, 1.f, true);
	QuadcopterRoot->GetBodyInstance()->PositionSolverIterationCount = 255;
	QuadcopterRoot->GetBodyInstance()->VelocitySolverIterationCount = 255;
	QuadcopterRoot->GetBodyInstance()->InertiaTensorScale = FVector::ZeroVector;
	QuadcopterRoot->GetBodyInstance()->StabilizationThresholdMultiplier = 0.f;
	RootComponent = QuadcopterRoot;

	QuadcopterModel = CreateDefaultSubobject<UStaticMeshComponent>("QuadcopterModel");
	QuadcopterModel->SetCollisionProfileName("NoCollision");
	QuadcopterModel->SetOwnerNoSee(true);
	QuadcopterModel->SetRelativeLocation(FVector(0.f, 0.f, -3.1f));
	QuadcopterModel->SetupAttachment(RootComponent);

	QuadcopterCamera = CreateDefaultSubobject<UCameraComponent>("QuadcopterCamera");
	QuadcopterCamera->bConstrainAspectRatio = true;
	QuadcopterCamera->FieldOfView = 130.f;
	QuadcopterCamera->SetRelativeRotation(FRotator(30.f, 0.f, 0.f));
	QuadcopterCamera->SetRelativeLocation(FVector(3.1f, 0.f, 1.1f));
	QuadcopterCamera->SetupAttachment(RootComponent);

	ThrottleInput = 0.f;
	ThrusterForce = 2000.f;
	RotationInput = FRotator::ZeroRotator;
	bHasRelativeRotation = false;
	MaxSpeed = 2500.f;
	ThrusterDistance = 7.f;
	SetMouseSensitivity(800.0, 8.0); 
	InitialLocation = FVector::ZeroVector;
	InitialRotation = FRotator::ZeroRotator;
	ThrusterOffset = -2.f;
	ThrottleExponent = 2.f;
	HitParameters = FCollisionQueryParams();
	HitParameters.AddIgnoredActor(this);
}
void AQuadcopter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	GEngine->GetEngineSubsystem<UGameInputSubsystem>()->GetQuadcopterInputs(
		ThrottleInput,
		RelativeInput.Roll,
		RelativeInput.Pitch,
		RelativeInput.Yaw
	);

	if (ThrottleInput != 0.f) {
		FVector InstantaneousForce;
		if (ThrusterDistance == 0.f) {
			InstantaneousForce = GetActorUpVector() * ThrottleInput * ThrusterForce;
			QuadcopterRoot->AddForce(InstantaneousForce * 4.f);
		}
		else {
			InstantaneousForce = FVector(0.f, 0.f, ThrottleInput * ThrusterForce );
			QuadcopterRoot->AddForceAtLocationLocal(InstantaneousForce, FVector(ThrusterDistance, ThrusterDistance, ThrusterOffset));
			QuadcopterRoot->AddForceAtLocationLocal(InstantaneousForce, FVector(-ThrusterDistance, ThrusterDistance, ThrusterOffset));
			QuadcopterRoot->AddForceAtLocationLocal(InstantaneousForce, FVector(ThrusterDistance, -ThrusterDistance, ThrusterOffset));
			QuadcopterRoot->AddForceAtLocationLocal(InstantaneousForce, FVector(-ThrusterDistance, -ThrusterDistance, ThrusterOffset));
		}
		// todo - ##TICKIMPROVEMENTS  dont use GetPhysicsLinearVelocity(), prefetch for cpu, etc. Everything in this tick function can be improved
		auto CurrentSpeed = QuadcopterRoot->GetPhysicsLinearVelocity().Size();
		if (CurrentSpeed > MaxSpeed) {
			QuadcopterRoot->SetPhysicsLinearVelocity(QuadcopterRoot->GetPhysicsLinearVelocity() / CurrentSpeed * MaxSpeed);
		}
	}
	//if (bHasRelativeRotation) {
		RelativeInput *= DeltaTime;
		RotationInput += RelativeInput;
		AddActorLocalRotation(RotationInput);
		RotationInput = FRotator::ZeroRotator;

		bHasRelativeRotation = false;
	//}
	auto CurrentLocation = RootComponent->GetComponentTransform().GetTranslation();
	auto Hit = FHitResult();
	GetWorld()->LineTraceSingleByChannel(
		Hit,
		CurrentLocation,
		PreviousLocation,
		ECollisionChannel::ECC_GameTraceChannel1,
		HitParameters
	);
	PreviousLocation = CurrentLocation;
	if (Hit.bBlockingHit) {
		if (CheckpointTriggered.IsBound()) CheckpointTriggered.Broadcast(FCheckpointEvent(Hit));
	}
}
void AQuadcopter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("KeyboardYaw", this, &AQuadcopter::KeyboardYaw);
	PlayerInputComponent->BindAxis("MousePitch", this, &AQuadcopter::MousePitch);
	PlayerInputComponent->BindAxis("MouseRoll", this, &AQuadcopter::MouseRoll);
	PlayerInputComponent->BindAction("RestartRun", IE_Pressed, this, &AQuadcopter::RestartRun);
	PlayerInputComponent->BindAction("RunDebugFunction", IE_Pressed, this, &AQuadcopter::RunDebugFunction);
}
void AQuadcopter::Throttle(float Input) {
	ThrottleInput = Input;
}
void AQuadcopter::KeyboardYaw(float Input) {
	if (Input != 0.f && !IsMoveInputIgnored()) {
		RelativeInput.Yaw += Input * 200.f;
		bHasRelativeRotation = true;
	}
}
void AQuadcopter::MousePitch(float Input) {
	if (Input != 0.f && !IsMoveInputIgnored()) {
		RotationInput.Pitch += Input * MouseSensitivity;
		bHasRelativeRotation = true;
	}
}
void AQuadcopter::MouseRoll(float Input) {
	if (Input != 0.f && !IsMoveInputIgnored()) {
		RotationInput.Roll += Input * MouseSensitivity;
		bHasRelativeRotation = true;
	}
}
void AQuadcopter::SetMouseSensitivity(double MouseDpi, double CentimetersPer360) {
	MouseSensitivity = 360.0 / MouseDpi / CentimetersPer360 * 2.54;
}
void AQuadcopter::RestartRun() {
	if (WantsRestartRun.IsBound()) WantsRestartRun.Broadcast();
}

void AQuadcopter::RunDebugFunction() {
}
void AQuadcopter::StopAllPhysics() {
	QuadcopterRoot->SetPhysicsLinearVelocity(FVector::ZeroVector);
	QuadcopterRoot->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
}


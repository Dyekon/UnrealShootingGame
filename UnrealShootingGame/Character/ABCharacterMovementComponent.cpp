// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ABCharacterMovementComponent.h"

UABCharacterMovementComponent::UABCharacterMovementComponent()
{
	bUseAccelerationForPaths = true;
	bOrientRotationToMovement = true;
	RotationRate = FRotator(0.0f, 1000.0f, 0.0f);
	JumpZVelocity = 700.f;
	AirControl = 0.35f;
	MaxWalkSpeed = 350.f;
	MinAnalogWalkSpeed = 100.f;
	BrakingDecelerationWalking = 2000.f;
	bRequestedMoveUseAcceleration = true;
}

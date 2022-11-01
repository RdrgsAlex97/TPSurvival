// Copyright Epic Games, Inc. All Rights Reserved.

#include "TPSurvivalCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"

//////////////////////////////////////////////////////////////////////////
// ATPSurvivalCharacter

ATPSurvivalCharacter::ATPSurvivalCharacter()
{
	bIsAiming = false;
	bIsSprinting = false;
	bIsTPP = true;

	movementSpeed = 500.0f;
	sneakMovementSpeed = 150.0f;
	sprintMovementSpeed = 750.0f;

	maxHealth = 100.0f;
	currentHealth = maxHealth;


	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rate for input
	TurnRateGamepad = 50.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a ThirdPersonCamera
	TPPCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("TPPCamera"));
	TPPCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	TPPCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
	

	//Create FirstPersonCamera
	FPPCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FPPCamera"));
	FPPCamera->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, "Head");
	FPPCamera->bUsePawnControlRotation = true; 
	
	

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

//////////////////////////////////////////////////////////////////////////
// Input

void ATPSurvivalCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &ATPSurvivalCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &ATPSurvivalCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Turn Right / Left Gamepad", this, &ATPSurvivalCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Look Up / Down Gamepad", this, &ATPSurvivalCharacter::LookUpAtRate);

	//Aim Down Sight
	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &ATPSurvivalCharacter::AimDownSight);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &ATPSurvivalCharacter::StopAimDownSight);

	//Sprint
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ATPSurvivalCharacter::StartSprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ATPSurvivalCharacter::StopSprint);

	//SwitchCamera third person to first and vice versa
	PlayerInputComponent->BindAction("SwitchCamera", IE_Pressed, this, &ATPSurvivalCharacter::SwitchCamera);

}


void ATPSurvivalCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void ATPSurvivalCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}



void ATPSurvivalCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ATPSurvivalCharacter::MoveRight(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void ATPSurvivalCharacter::AimDownSight()
{

	if (auto camera = GetCameraBoom()) {

		camera->TargetArmLength = 50.0f;
		camera->TargetOffset = FVector(0.0f, 30.0f, 70.0f);
		TPPCamera->bUsePawnControlRotation = true;
		
		if (auto characterMovement = GetCharacterMovement()) {

			characterMovement->MaxWalkSpeed = sneakMovementSpeed;

			if (bIsTPP)
			{

			this->bUseControllerRotationYaw = true;
			GetCharacterMovement()->bUseControllerDesiredRotation = true;
			GetCharacterMovement()->bOrientRotationToMovement = false;
			}

		}


	}

	bIsAiming = true;

}

void ATPSurvivalCharacter::StopAimDownSight()
{

	if (auto camera = GetCameraBoom()) {

		camera->TargetArmLength = 300.0f;
		camera->TargetOffset = FVector(0.0f, 0.0f, 0.0f);
		TPPCamera->bUsePawnControlRotation = false;

		if (auto characterMovement = GetCharacterMovement()) {

			characterMovement->MaxWalkSpeed = movementSpeed;

			if (bIsTPP) {

				this->bUseControllerRotationYaw = false;
				GetCharacterMovement()->bUseControllerDesiredRotation = false;
				GetCharacterMovement()->bOrientRotationToMovement = true;
			}
		}

	}

	bIsAiming = false;


}

void ATPSurvivalCharacter::StartSprint()
{

	if (!bIsAiming)
	{
		if (auto characterMovement = GetCharacterMovement()) {

			characterMovement->MaxWalkSpeed = sprintMovementSpeed;
			TPPCamera->SetFieldOfView(100.0f);
		}
		
	}

}

void ATPSurvivalCharacter::StopSprint()
{
	if (bIsAiming)
		return;

	if (auto characterMovement = GetCharacterMovement()) {

		characterMovement->MaxWalkSpeed = movementSpeed;
		TPPCamera->SetFieldOfView(90.0f);
	}

}

void ATPSurvivalCharacter::SwitchCamera(){


	if(bIsTPP){

		TPPCamera->SetActive(false);
		FPPCamera->SetActive(true);

		GetCharacterMovement()->bUseControllerDesiredRotation = true;
		GetCharacterMovement()->bOrientRotationToMovement = false;

		bIsTPP = false;
	
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("First Person"));


	}else{

		bIsTPP = true;
		TPPCamera->SetActive(true);
		FPPCamera->SetActive(false);

		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;


		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Third Person"));

	}



}


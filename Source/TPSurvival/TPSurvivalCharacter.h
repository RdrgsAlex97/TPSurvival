// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TPSurvivalCharacter.generated.h"

UCLASS(config = Game)
class ATPSurvivalCharacter : public ACharacter
{
	GENERATED_BODY()

		/** Camera boom positioning the camera behind the character */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	/** Third Person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* TPPCamera;

	/** First Person Camera */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FPPCamera;

	/** Mesh da arma */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon, meta = (AllowPrivateAccess = "true"))
		class USkeletalMesh* WeaponMesh;

	

public:
	ATPSurvivalCharacter();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Input)
		float TurnRateGamepad;

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return TPPCamera; }

	//FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FPPCamera; }

protected:

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);



private:


public:


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera)
		bool bIsAiming;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = CharacterMovement)
		bool bIsSprinting;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
		bool bIsTPP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CharacterMovement)
		float movementSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CharacterMovement)
		float sneakMovementSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CharacterMovement)
		float sprintMovementSpeed;

	


protected:

	void AimDownSight();
	void StopAimDownSight();

	void StartSprint();
	void StopSprint();

	void SwitchCamera();

};



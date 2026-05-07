// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BaseCharacter.h"
#include "Combat/CombatTypes.h"
#include "PlayerCharacter.generated.h"

class UInputMappingContext;
class USpringArmComponent;
class UCameraComponent;
class UInputAction;
class UInputBufferComponent;
class UCameraShakeBase;
class UCurveFloat;
class UAIPerceptionStimuliSourceComponent;
class UStaminaComponent;
struct FInputActionValue;

UCLASS()
class COMBATCORE_API APlayerCharacter : public ABaseCharacter
{
	GENERATED_BODY()
	
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputBufferComponent> InputBufferComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAIPerceptionStimuliSourceComponent> StimuliSourceComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaminaComponent> StaminaComponent;
	
	FVector2D LastMovementInput = FVector2D::ZeroVector;
	
protected:
	
	/** Input Mapping Context **/
	UPROPERTY(EditAnywhere, Category="Input")
	UInputMappingContext* DefaultMappingContext;
	
	/** Jump Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* LookAction;
	
	/** Light Attack Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* LightAttackAction;
	
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* HeavyAttackAction;
	
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* DodgeAction;

	UPROPERTY(EditAnywhere, Category="Dodge")
	float DodgeSpeed = 1000.f;

	UPROPERTY(EditAnywhere, Category="Dodge")
	TObjectPtr<UCurveFloat> DodgeCurve;

	UPROPERTY(EditAnywhere, Category="Dodge")
	float DodgeDuration = 0.45f;

	FVector CachedDodgeDirection = FVector::ZeroVector;
	float DodgeElapsedTime = 0.f;


public:

	/** Constructor */
	APlayerCharacter();	
	
protected:
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	/** Initialize input action bindings */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);
	
	void LightAttack(const FInputActionValue& Value);
	
	void HeavyAttack(const FInputActionValue& Value);
	
	void Dodge(const FInputActionValue& Value);
	
	// Move 키 뗀 시점 감지 - LastMovementInput 초기화
	void OnMoveCompleted(const FInputActionValue& Value);

public:

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	
	FORCEINLINE UInputBufferComponent* GetInputBufferComponent() const { return InputBufferComponent; }
	
	FORCEINLINE UStaminaComponent* GetStaminaComponent() const { return StaminaComponent; }
	
	void PlayHitCameraShake(TSubclassOf<UCameraShakeBase> ShakeClass, float Scale = 1.f);
};

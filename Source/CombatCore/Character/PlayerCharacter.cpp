// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/PlayerCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedPlayerInput.h"
#include "InputActionValue.h"
#include "Combat/CombatComponent.h"

APlayerCharacter::APlayerCharacter()
{	
	// 컨트롤러(카메라) 회전과 캐릭터 독립
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	// 이동하는 방향으로 회전
	GetCharacterMovement()->bOrientRotationToMovement = true;
	
	// 카메라 설정
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;
	
	CameraBoom->bDoCollisionTest= true;
	CameraBoom->SetRelativeLocation(FVector(0.0f, 0.0f, 40.0f));
	
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
		
		// Pitch 제한
		PC->PlayerCameraManager->ViewPitchMin = -60.f;
		PC->PlayerCameraManager->ViewPitchMax = 30.f;
	}
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{	
		// 이동
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
		
		// 시점 회전
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);
		
		// 점프
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		
		// 경공격
		EnhancedInputComponent->BindAction(LightAttackAction, ETriggerEvent::Started, this, &APlayerCharacter::LightAttack);
		
		//강공격
		EnhancedInputComponent->BindAction(HeavyAttackAction, ETriggerEvent::Started, this, &APlayerCharacter::HeavyAttack);
	}
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
	if (GetController() == nullptr) return;
	
	// ActionInputValue은 Vector 2D
	FVector2D MovementVector = Value.Get<FVector2D>();
	
	// 컨트롤러의 rotation (Yaw)
	const FRotator Rotation = GetController()->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);
	
	// forward, right vector
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	
	// Add Movement
	AddMovementInput(ForwardDirection, MovementVector.Y);
	AddMovementInput(RightDirection, MovementVector.X);
	
}

void APlayerCharacter::Look(const FInputActionValue& Value)
{	
	if (GetController() == nullptr) return;
	
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	
	// 컨트롤러(카메라)에 회전 적용
	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(-LookAxisVector.Y);
	
}

void APlayerCharacter::LightAttack(const FInputActionValue& Value)
{
	if (UCombatComponent* Combat = GetCombatComponent())
	{
		Combat->HandleCombatInput(EInputType::Light);
	}
}

void APlayerCharacter::HeavyAttack(const FInputActionValue& Value)
{
	if (UCombatComponent* Combat = GetCombatComponent())
	{
		Combat->HandleCombatInput(EInputType::Heavy);
	}
}


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
#include "Combat/InputBufferComponent.h"
#include "Combat/CombatAbility.h"
#include "Curves/CurveFloat.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

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
	
	InputBufferComponent = CreateDefaultSubobject<UInputBufferComponent>(TEXT("InputBufferComponent"));
	
	StimuliSourceComponent = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("StimuliSourceComponent"));
	StimuliSourceComponent->RegisterForSense(TSubclassOf<UAISense>(UAISense_Sight::StaticClass()));
	StimuliSourceComponent->bAutoRegister = true;
	
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
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &APlayerCharacter::OnMoveCompleted);
		
		// 시점 회전
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);
		
		// 점프
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		
		// 경공격
		EnhancedInputComponent->BindAction(LightAttackAction, ETriggerEvent::Started, this, &APlayerCharacter::LightAttack);
		
		//강공격
		EnhancedInputComponent->BindAction(HeavyAttackAction, ETriggerEvent::Started, this, &APlayerCharacter::HeavyAttack);
		
		//회피
		EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Started, this, &APlayerCharacter::Dodge);
	}
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
	if (GetController() == nullptr) return;

	FVector2D MovementVector = Value.Get<FVector2D>();

	LastMovementInput = MovementVector;

	if (UCombatComponent* Combat = GetCombatComponent())
	{
		if (Combat->GetState() != ECombatState::Idle) return;
	}

	const FRotator Rotation = GetController()->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

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

void APlayerCharacter::Dodge(const FInputActionValue& Value)
{
	UCombatComponent* Combat = GetCombatComponent();
	if (!Combat) return;

	TSubclassOf<UCombatAbility> DodgeClass = Combat->GetDodgeAbilityClass();
	if (!DodgeClass) return;

	const FRotator CameraYawRot(0, GetControlRotation().Yaw, 0);
	const FVector Forward = FRotationMatrix(CameraYawRot).GetUnitAxis(EAxis::X);
	const FVector Right   = FRotationMatrix(CameraYawRot).GetUnitAxis(EAxis::Y);

	FVector DodgeVec;
	if (LastMovementInput.IsNearlyZero())
	{
		DodgeVec = -Forward;
	}
	else
	{
		DodgeVec = (Forward * LastMovementInput.Y + Right * LastMovementInput.X).GetSafeNormal();
	}

	SetActorRotation(CameraYawRot);

	CachedDodgeDirection = DodgeVec;
	DodgeElapsedTime = 0.f;

	Combat->ExecuteAbility(DodgeClass);
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (UCombatComponent* Combat = GetCombatComponent())
	{
		if (Combat->GetState() == ECombatState::Dodging)
		{
			DodgeElapsedTime += DeltaTime;
			const float Alpha = FMath::Clamp(DodgeElapsedTime / DodgeDuration, 0.f, 1.f);
			const float SpeedMultiplier = DodgeCurve ? DodgeCurve->GetFloatValue(Alpha) : 1.f;
			GetCharacterMovement()->Velocity = CachedDodgeDirection * DodgeSpeed * SpeedMultiplier;
		}
		else if (Combat->GetState() == ECombatState::Attacking)
		{
			const FVector Impulse = Combat->GetAttackImpulseVelocity(DeltaTime);
			if (!Impulse.IsZero())
			{
				GetCharacterMovement()->Velocity = FVector(Impulse.X, Impulse.Y, GetCharacterMovement()->Velocity.Z); 
			}
		}
	}
}

void APlayerCharacter::OnMoveCompleted(const FInputActionValue& Value)
{
	LastMovementInput = FVector2D::ZeroVector;
}

void APlayerCharacter::PlayHitCameraShake(TSubclassOf<UCameraShakeBase> ShakeClass, float Scale)
{
	if (!ShakeClass) return;
	
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		PC->ClientStartCameraShake(ShakeClass, Scale);
	}
}

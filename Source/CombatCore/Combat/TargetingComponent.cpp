
#include "Combat/TargetingComponent.h"
#include "Character/BaseCharacter.h"
#include "Combat/HealthComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/UserWidget.h"
#include "EngineUtils.h"


UTargetingComponent::UTargetingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UTargetingComponent::ToggleLockOn()
{
	if (bIsLockedOn)
	{
		ReleaseLockOn();
	}
	else
	{
		if (AActor* BestTarget = FindBestTarget())
		{
			LockOnTo(BestTarget);
		}
	}
}

void UTargetingComponent::LockOnTo(AActor* Target)
{
	LockedTarget = Target;
	bIsLockedOn = true;
	LOSLostTimer = 0.f;
	
	SetComponentTickEnabled(true);
	
	if (ACharacter* OwnerChar = Cast<ACharacter>(GetOwner()))
	{
		OwnerChar->GetCharacterMovement()->bOrientRotationToMovement = false;
		OwnerChar->GetCharacterMovement()->bUseControllerDesiredRotation = true;
	}
	
	if (ABaseCharacter* BaseTarget = Cast<ABaseCharacter>(Target))
	{
		if (UHealthComponent* HealthComp = BaseTarget->GetHealthComponent())
		{
			HealthComp->OnDeath.AddDynamic(this, &UTargetingComponent::HandleTargetDeath);
		}
	}
	
	CreateIndicatorWidget(Target);
}

void UTargetingComponent::ReleaseLockOn()
{
	if (LockedTarget.IsValid())
	{
		if (ABaseCharacter* BaseTarget = Cast<ABaseCharacter>(LockedTarget.Get()))
		{
			if (UHealthComponent* HealthComp = BaseTarget->GetHealthComponent())
			{
				HealthComp->OnDeath.RemoveDynamic(this, &UTargetingComponent::HandleTargetDeath);
			}
		}
	}
	
	DestroyIndicatorWidget();
	
	LockedTarget = nullptr;
	bIsLockedOn = false;
	LOSLostTimer = 0.f;
	
	SetComponentTickEnabled(false);
	
	if (ACharacter* OwnerChar = Cast<ACharacter>(GetOwner()))
	{
		OwnerChar->GetCharacterMovement()->bOrientRotationToMovement = true;
		OwnerChar->GetCharacterMovement()->bUseControllerDesiredRotation = false;
	}
}

void UTargetingComponent::HandleTargetDeath(const FDamageInfo& DamageInfo)
{
	ReleaseLockOn();
}

void UTargetingComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                        FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (!LockedTarget.IsValid() || !IsTargetValid(LockedTarget.Get()))
	{
		ReleaseLockOn();
		return;
	}
	
	if (!HasLineOfSightTo(LockedTarget.Get()))
	{
		LOSLostTimer += DeltaTime;
		if (LOSLostTimer >= LOSLostGracePeriod)
		{
			ReleaseLockOn();
			return;
		}
	}
	else
	{
		LOSLostTimer = 0.f;
	}
	
	UpdateCameraRotation(DeltaTime);
}

void UTargetingComponent::UpdateCameraRotation(float DeltaTime)
{
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn || !OwnerPawn->GetController()) return;
	
	const FVector CameraLocation = OwnerPawn->GetPawnViewLocation();
	const FVector TargetLocation = LockedTarget->GetActorLocation();
	const FRotator DesireRotation = (TargetLocation - CameraLocation).Rotation();
	
	const FRotator CurrentRotation = OwnerPawn->GetController()->GetControlRotation();
	const FRotator SmoothRotation = FMath::RInterpTo(CurrentRotation, DesireRotation, DeltaTime, InterpSpeed);
	
	OwnerPawn->GetController()->SetControlRotation(SmoothRotation);
}

TArray<AActor*> UTargetingComponent::GatherCandidates() const
{
	TArray<AActor*> Candidates;
	const FVector OwnerLocation = GetOwner()->GetActorLocation();
	
	for (TActorIterator<ABaseCharacter> It(GetWorld()); It; ++It)
	{
		ABaseCharacter* Candidate = *It;
		if (Candidate == GetOwner()) continue;
		
		UHealthComponent* HealthComp = Candidate->GetHealthComponent();
		if (!HealthComp || !HealthComp->IsAlive()) continue;
		
		const float Distance = FVector::Dist(OwnerLocation, Candidate->GetActorLocation());
		if (Distance > MaxTargetingRange) continue;
		
		Candidates.Add(Candidate);
	}
	
	return Candidates;
}

float UTargetingComponent::ScoreTarget(AActor* Candidate) const
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC || !PC->PlayerCameraManager) return TNumericLimits<float>::Max();
	
	const FVector CameraForward = PC->PlayerCameraManager->GetActorForwardVector();
	const FVector ToCandidate = (Candidate->GetActorLocation() - GetOwner()->GetActorLocation()).GetSafeNormal();
	
	const float Angle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(CameraForward, ToCandidate)));
	if (Angle > MaxTargetAngle) return TNumericLimits<float>::Max();
	
	const float Distance = FVector::Dist(Candidate->GetActorLocation(), GetOwner()->GetActorLocation());
	
	FVector2D ScreenPos;
	PC->ProjectWorldLocationToScreen(Candidate->GetActorLocation(), ScreenPos);
	
	int32 ViewportX, ViewportY;
	PC->GetViewportSize(ViewportX, ViewportY);
	const FVector2D ScreenCenter(ViewportX * 0.5f, ViewportY * 0.5f);
	const float ScreenDist = FVector2D::Distance(ScreenPos, ScreenCenter);
	
	const float NormalizedDist = Distance / MaxTargetingRange;
	const float NormalizedScreen = ScreenDist / FMath::Max(ViewportX * 0.5f, 1.f);
	
	return NormalizedDist * 0.4f + NormalizedScreen * 0.6f;
}

AActor* UTargetingComponent::FindBestTarget() const
{
	TArray<AActor*> Candidates = GatherCandidates();
	AActor* BestTarget = nullptr;
	float BestScore = TNumericLimits<float>::Max();
	
	for (AActor* Candidate : Candidates)
	{
		const float Score = ScoreTarget(Candidate);
		if (Score < BestScore)
		{
			BestScore = Score;
			BestTarget = Candidate;
		}
	}
	
	return BestTarget;
}

void UTargetingComponent::SwitchTarget(float Direction)
{
	if (!bIsLockedOn || FMath::IsNearlyZero(Direction)) return;
	
	TArray<AActor*> Candidates = GatherCandidates();
	
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC || !PC->PlayerCameraManager) return;
	
	const FVector CameraRight = PC->PlayerCameraManager->GetActorRightVector();
	const FVector OwnerLocation = GetOwner()->GetActorLocation();
	
	AActor* BestCandidate = nullptr;
	float BestAngle = TNumericLimits<float>::Max();
	
	for (AActor* Candidate : Candidates)
	{
		if (Candidate == LockedTarget.Get()) continue;
		
		const FVector ToCandidate = (Candidate->GetActorLocation() - OwnerLocation).GetSafeNormal();
		const float Dot = FVector::DotProduct(ToCandidate, CameraRight);
		
		if ((Direction > 0.f && Dot <= 0.f) || (Direction < 0.f && Dot >= 0.f)) continue;
		
		const FVector CameraForward = PC->PlayerCameraManager->GetActorForwardVector();
		const float Angle = FMath::Abs(FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(CameraForward, ToCandidate))));
		
		if (Angle < BestAngle)
		{
			BestAngle = Angle;
			BestCandidate = Candidate;
		}
	}
	
	if (BestCandidate)
	{
		ReleaseLockOn();
		LockOnTo(BestCandidate);
	}
}

bool UTargetingComponent::IsTargetValid(AActor* Target) const
{
	if (!IsValid(Target)) return false;
	
	const float Distance = FVector::Dist(GetOwner()->GetActorLocation(), Target->GetActorLocation());
	if (Distance > MaxTargetingRange) return false;
	
	if (const ABaseCharacter* BaseTarget = Cast<ABaseCharacter>(Target))
	{
		if (const UHealthComponent* HealthComp = BaseTarget->GetHealthComponent())
		{
			return HealthComp->IsAlive();
		}
	}
	return false;
}

bool UTargetingComponent::HasLineOfSightTo(AActor* Target) const
{
	const APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn) return false;
	
	const FVector Start = OwnerPawn->GetPawnViewLocation();
	const FVector End = Target->GetActorLocation() + FVector(0.f, 0.f, TargetHeightOffset);
	
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());
	Params.AddIgnoredActor(Target);
	
	FHitResult Hit;
	return !GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);
	
}

void UTargetingComponent::CreateIndicatorWidget(AActor* Target)
{
	if (!LockOnIndicatorClass || !Target) return;
	
	IndicatorWidgetComponent = NewObject<UWidgetComponent>(Target);
	IndicatorWidgetComponent->RegisterComponent();
	IndicatorWidgetComponent->AttachToComponent(
		Target->GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	IndicatorWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	IndicatorWidgetComponent->SetDrawAtDesiredSize(true);
	IndicatorWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, TargetHeightOffset));
	IndicatorWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	IndicatorWidgetComponent->SetWidgetClass(LockOnIndicatorClass);
}

void UTargetingComponent::DestroyIndicatorWidget()
{
	if (IndicatorWidgetComponent)
	{
		IndicatorWidgetComponent->DestroyComponent();
		IndicatorWidgetComponent = nullptr;
	}
}

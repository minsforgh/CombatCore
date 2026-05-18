
#include "AI/BTService_FindPlayer.h"
#include "EnemyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/BaseCharacter.h"
#include "Combat/CombatComponent.h"
#include "Combat/HealthComponent.h"
#include "AI/EnemyAITypes.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/CharacterMovementComponent.h"

UBTService_FindPlayer::UBTService_FindPlayer()
{
	NodeName = "FindPlayer";
	Interval = 0.1f;
	RandomDeviation = 0.02f;
}

void UBTService_FindPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) return;
	
	AEnemyAIController* EIC = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());
	APawn* Pawn = EIC ? EIC->GetPawn() : nullptr;
	
	UCharacterMovementComponent* Movement = nullptr;
	if (ACharacter* Character = Cast<ACharacter>(Pawn))
	{
		Movement = Character->GetCharacterMovement();
	}

	auto EnterIdle = [&]()
	{
		BB->SetValueAsObject(TargetActorKey.SelectedKeyName, nullptr);
		BB->SetValueAsEnum(AIStateKey.SelectedKeyName, static_cast<uint8>(EEnemyAIState::Idle));
		EIC->ClearFocus(EAIFocusPriority::Gameplay);
		if (Movement)
		{
			Movement->bOrientRotationToMovement = false;
			Movement->bUseControllerDesiredRotation = false;
		}
	};

	if (!Pawn)
	{
		BB->SetValueAsEnum(AIStateKey.SelectedKeyName, static_cast<uint8>(EEnemyAIState::Idle));
		return;
	}

#if ENABLE_DRAW_DEBUG
	if (EIC->IsShowingDebug())
	{
		DrawDebugCircle(GetWorld(), Pawn->GetActorLocation(), AttackRange, 32, FColor::Red, false, Interval, 0, 1.5f,
			FVector(1, 0, 0), FVector(0, 1, 0), false);
	}
#endif

	UObject* TargetObj = BB->GetValueAsObject(TargetActorKey.SelectedKeyName);
	AActor* Target = Cast<AActor>(TargetObj);

	if (!IsValid(Target))
	{
		EnterIdle();
		return;
	}

	if (ABaseCharacter* TargetChar = Cast<ABaseCharacter>(Target))
	{
		if (UHealthComponent* HP = TargetChar->GetHealthComponent())
		{
			if (!HP->IsAlive())
			{
				EnterIdle();
				return;
			}
		}
	}

	const bool bSeen = BB->GetValueAsBool(bCurrentlySeenKey.SelectedKeyName);
	const float LastSeenTime = BB->GetValueAsFloat(LastSeenTimeKey.SelectedKeyName);
	const float Now = GetWorld()->GetTimeSeconds();
	const float TimeSinceLost = Now - LastSeenTime;

	if (!bSeen && TimeSinceLost > LoseGracePeriod)
	{
		EnterIdle();
		return;
	}

	EIC->SetFocus(Target);

	const float Distance = FVector::Dist(Pawn->GetActorLocation(), Target->GetActorLocation());
	const bool bOnCooldown = (Now - EIC->GetLastAttackTime()) < AttackCooldown;

	bool bIsAttacking = false;
	if (ABaseCharacter* BaseChar = Cast<ABaseCharacter>(Pawn))
	{
		if (UCombatComponent* Combat = BaseChar->GetCombatComponent())
		{
			bIsAttacking = (Combat->GetState() == ECombatState::Attacking);
		}
	}

	EEnemyAIState NewState;

	if (!bSeen)
	{
		NewState = EEnemyAIState::Chase;
	}
	else if (Distance <= AttackRange)
	{
		NewState = bOnCooldown ? EEnemyAIState::Wait : EEnemyAIState::Attack;
	}
	else
	{
		NewState = EEnemyAIState::Chase;
	}

	BB->SetValueAsEnum(AIStateKey.SelectedKeyName, static_cast<uint8>(NewState));

	if (Movement)
	{
		if (bIsAttacking)
		{
			Movement->StopMovementImmediately();
			Movement->bOrientRotationToMovement = false;
			Movement->bUseControllerDesiredRotation = true;
			Movement->RotationRate.Yaw = 0.f;
		}
		else
		{
			const bool bChasing = (NewState == EEnemyAIState::Chase);
			Movement->bOrientRotationToMovement = bChasing;
			Movement->bUseControllerDesiredRotation = !bChasing;
			float NewYawRate;
			if (NewState == EEnemyAIState::Attack)
				NewYawRate = 0.f;
			else if (NewState == EEnemyAIState::Wait)
				NewYawRate = 180.f;
			else
				NewYawRate = 360.f;
			Movement->RotationRate.Yaw = NewYawRate;
		}
	}
}

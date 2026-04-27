
#include "AI/BTService_FindPlayer.h"
#include "EnemyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/BaseCharacter.h"
#include "Combat/HealthComponent.h"
#include "AI/EnemyAITypes.h"
#include "DrawDebugHelpers.h"

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
		BB->SetValueAsObject(TargetActorKey.SelectedKeyName, nullptr);
		BB->SetValueAsEnum(AIStateKey.SelectedKeyName, static_cast<uint8>(EEnemyAIState::Idle));
		return;
	}

	if (ABaseCharacter* TargetChar = Cast<ABaseCharacter>(Target))
	{
		if (UHealthComponent* HP = TargetChar->GetHealthComponent())
		{
			if (!HP->IsAlive())
			{
				BB->SetValueAsObject(TargetActorKey.SelectedKeyName, nullptr);
				BB->SetValueAsEnum(AIStateKey.SelectedKeyName, static_cast<uint8>(EEnemyAIState::Idle));
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
		BB->SetValueAsEnum(AIStateKey.SelectedKeyName, static_cast<uint8>(EEnemyAIState::Idle));
		BB->SetValueAsObject(TargetActorKey.SelectedKeyName, nullptr);
		return;
	}

	const float Distance = FVector::Dist(Pawn->GetActorLocation(), Target->GetActorLocation());
	const bool bOnCooldown = (Now - EIC->GetLastAttackTime()) < AttackCooldown;

	if (!bSeen)
	{
		BB->SetValueAsEnum(AIStateKey.SelectedKeyName, static_cast<uint8>(EEnemyAIState::Chase));
		return;
	}

	if (Distance <= AttackRange)
	{
		if (bOnCooldown)
		{
			BB->SetValueAsEnum(AIStateKey.SelectedKeyName, static_cast<uint8>(EEnemyAIState::Wait));
		}
		else
		{
			BB->SetValueAsEnum(AIStateKey.SelectedKeyName, static_cast<uint8>(EEnemyAIState::Attack));
		}
	}
	else
	{
		BB->SetValueAsEnum(AIStateKey.SelectedKeyName, static_cast<uint8>(EEnemyAIState::Chase));
	}
}

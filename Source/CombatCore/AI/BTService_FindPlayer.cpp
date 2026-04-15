
#include "AI/BTService_FindPlayer.h"
#include "AIController.h"
#include "EnemyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/BaseCharacter.h"
#include "Combat/HealthComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "AI/EnemyAITypes.h"

UBTService_FindPlayer::UBTService_FindPlayer()
{
	NodeName = "FindPlayer";
	Interval = 0.5f;
	RandomDeviation = 0.1f;
}

void UBTService_FindPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) return;
	
	AAIController* AIOwner = OwnerComp.GetAIOwner();
	APawn* Pawn = AIOwner ? AIOwner->GetPawn() : nullptr;
	
	ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	bool bIsPlayerAlive = false;
	if (ABaseCharacter* BaseChar = Cast<ABaseCharacter>(Player))
	{
		if (UHealthComponent* HealthComp = BaseChar->GetHealthComponent())
		{
			bIsPlayerAlive = HealthComp->IsAlive();
		}
	}
	
	if (!Pawn || !Player || !bIsPlayerAlive)
	{
		BB->SetValueAsEnum(AIStateKey.SelectedKeyName, (uint8)EEnemyAIState::Idle);
		BB->SetValueAsObject(TargetActorKey.SelectedKeyName, nullptr);
		return;
	}
	
	float Distance = FVector::Dist(Pawn->GetActorLocation(), Player->GetActorLocation());
	float CurrentTime = GetWorld()->GetTimeSeconds();
	bool bOnCooldown = false;
	
	if (AEnemyAIController* EIC = Cast<AEnemyAIController>(AIOwner))
	{
		bOnCooldown = (CurrentTime - EIC->GetLastAttackTime()) < AttackCooldown;
	}
	
	if (Distance > DetectionRange)
	{
		BB->SetValueAsEnum(AIStateKey.SelectedKeyName, (uint8)EEnemyAIState::Idle);
		BB->SetValueAsObject(TargetActorKey.SelectedKeyName, nullptr);
		return;
	}
	
	BB->SetValueAsObject(TargetActorKey.SelectedKeyName, Player);
	
	if (bOnCooldown || Distance < MaintainDistance)
	{
		BB->SetValueAsEnum(AIStateKey.SelectedKeyName, (uint8)EEnemyAIState::Wait);
	}
	else if (Distance <= AttackRange)
	{
		BB->SetValueAsEnum(AIStateKey.SelectedKeyName, (uint8)EEnemyAIState::Attack);
	}
	else
	{
		BB->SetValueAsEnum(AIStateKey.SelectedKeyName, (uint8)EEnemyAIState::Chase);
	}
	
	
}

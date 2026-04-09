// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTService_FindPlayer.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

UBTService_FindPlayer::UBTService_FindPlayer()
{
	NodeName = "FindPlayer";
	Interval = 0.5f;
	RandomDeviation = 0.1f;
}

void UBTService_FindPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	
	AAIController* AIOwner = OwnerComp.GetAIOwner();
	APawn* Pawn = AIOwner ? AIOwner->GetPawn() : nullptr;
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

	if (Pawn && PlayerCharacter)
	{
		BB->SetValueAsObject(TargetActorKey.SelectedKeyName, PlayerCharacter);

		const float Distance = FVector::Dist(Pawn->GetActorLocation(), PlayerCharacter->GetActorLocation());
		BB->SetValueAsBool(bIsInAttackRangeKey.SelectedKeyName, Distance <= AttackRange);
	}
	else
	{
		BB->ClearValue(TargetActorKey.SelectedKeyName);
		BB->SetValueAsBool(bIsInAttackRangeKey.SelectedKeyName, false);
	}
}

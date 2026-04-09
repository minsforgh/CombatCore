
#include "AI/BTTask_Attack.h"
#include "AIController.h"
#include "Combat/CombatComponent.h"
#include "Character/BaseCharacter.h"


UBTTask_Attack::UBTTask_Attack()
{
	NodeName = "Attack";
	// AI마다 독립 인스턴스 — Cached 포인터가 다른 AI와 공유되는 것을 방지
	bCreateNodeInstance = true;
	bNotifyTaskFinished = true;
}

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* Controller = OwnerComp.GetAIOwner();
	APawn* Pawn = Controller ? Controller->GetPawn() : nullptr;
	ABaseCharacter* Character = Cast<ABaseCharacter>(Pawn);
	
	if (!Character) return EBTNodeResult::Failed;
	
	UCombatComponent* CombatComponent = Character->GetCombatComponent();
	if (!CombatComponent) return EBTNodeResult::Failed;
	
	CachedCombatComponent = CombatComponent;
	CachedOwnerComponent = &OwnerComp;
	
	if (!bIsListening)
	{
		CombatComponent->OnCombatStateChanged.AddDynamic(this, &UBTTask_Attack::OnCombatStateChanged);
		bIsListening = true;
	}
	
	CombatComponent->HandleCombatInput(InputType);

	return EBTNodeResult::InProgress;
}

void UBTTask_Attack::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	EBTNodeResult::Type TaskResult)
{	
	if (bIsListening && CachedCombatComponent.IsValid())
	{
		CachedCombatComponent->OnCombatStateChanged.RemoveDynamic(this, &UBTTask_Attack::OnCombatStateChanged);
		bIsListening = false;
	}
	CachedOwnerComponent = nullptr;
	
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
}

void UBTTask_Attack::OnCombatStateChanged(ECombatState OldState, ECombatState NewState)
{
	if (!CachedCombatComponent.IsValid() || !CachedOwnerComponent.IsValid()) return;
	
	if (NewState == ECombatState::Idle)
	{
		FinishLatentTask(*CachedOwnerComponent.Get(), EBTNodeResult::Succeeded);
	}
	else if (NewState == ECombatState::Dead)
	{
		FinishLatentTask(*CachedOwnerComponent.Get(), EBTNodeResult::Failed);
	}
}

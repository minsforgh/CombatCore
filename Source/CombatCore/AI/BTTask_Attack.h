
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Combat/CombatTypes.h"
#include "BTTask_Attack.generated.h"

class UCombatComponent;
class UBehaviorTreeComponent;

UCLASS()
class COMBATCORE_API UBTTask_Attack : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_Attack();
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;
	
	UPROPERTY(EditAnywhere, Category = "Attack")
	EInputType InputType = EInputType::Light;
	
private:
	UFUNCTION()
	void OnCombatStateChanged(ECombatState OldState, ECombatState NewState);
	
	TWeakObjectPtr<UCombatComponent> CachedCombatComponent;
	TWeakObjectPtr<UBehaviorTreeComponent> CachedOwnerComponent;
	bool bIsListening = false;
};

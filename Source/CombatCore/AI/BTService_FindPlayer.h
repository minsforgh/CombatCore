#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_FindPlayer.generated.h"


UCLASS()
class COMBATCORE_API UBTService_FindPlayer : public UBTService
{
	GENERATED_BODY()

public:
	
	UBTService_FindPlayer();
	
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetActorKey;
	
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector AIStateKey;
	
	UPROPERTY(EditAnywhere, Category = "Distance")
	float DetectionRange = 1000.f;
	
	UPROPERTY(EditAnywhere, Category = "Distance")
	float AttackRange = 200.f;
	
	UPROPERTY(EditAnywhere, Category = "Attack")
	float AttackCooldown = 2.0f;

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};

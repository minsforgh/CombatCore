#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

class UBehaviorTree;

UCLASS()
class COMBATCORE_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	float GetLastAttackTime() const {return LastAttackTime; }
	void SetLastAttackTime(float Time) {LastAttackTime = Time;}
	
protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UBehaviorTree> BehaviorTreeAsset;
	
	float LastAttackTime = -FLT_MAX;
};

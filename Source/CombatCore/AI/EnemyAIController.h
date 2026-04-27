#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

class UBehaviorTree;
class UAISenseConfig_Sight;

UCLASS()
class COMBATCORE_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	AEnemyAIController();

	float GetLastAttackTime() const { return LastAttackTime; }
	void SetLastAttackTime(float Time) { LastAttackTime = Time; }
	bool IsShowingDebug() const { return bShowDebug; }

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UBehaviorTree> BehaviorTreeAsset;

	UPROPERTY()
	TObjectPtr<UAISenseConfig_Sight> SightConfig;

	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	UPROPERTY(EditAnywhere, Category = "Debug", meta = (AllowPrivateAccess = true))
	bool bShowDebug = false;

	float LastAttackTime = -FLT_MAX;		
};

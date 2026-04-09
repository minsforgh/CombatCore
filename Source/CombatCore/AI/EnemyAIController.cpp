#include "AI/EnemyAIController.h"
#include "BehaviorTree/BehaviorTree.h"

void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();
	
	if (BehaviorTreeAsset)
	{
		RunBehaviorTree(BehaviorTreeAsset);
	}
}

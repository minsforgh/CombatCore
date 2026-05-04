
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_ConsumeWindow.generated.h"


// 이 구간 동안 InputBuffer에 쌓인 입력을 소비하여 다음 콤보 스텝으로 진행한다
// 몽타주 타임라인에서 구간 배치로 콤보 연계 타이밍을 제어
UCLASS()
class COMBATCORE_API UAnimNotifyState_ConsumeWindow : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration,
		const FAnimNotifyEventReference& EventReference) override;
	
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference) override;
	
	virtual FString GetNotifyName_Implementation() const override;
};

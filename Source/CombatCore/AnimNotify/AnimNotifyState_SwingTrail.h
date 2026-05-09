
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_SwingTrail.generated.h"

class UNiagaraSystem;
class UNiagaraComponent;

UCLASS()
class COMBATCORE_API UAnimNotifyState_SwingTrail : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration,
								 const FAnimNotifyEventReference& EventReference) override;

	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
						   const FAnimNotifyEventReference& EventReference) override;

	virtual FString GetNotifyName_Implementation() const override;

private:
	UPROPERTY(EditAnywhere, Category = "FX")
	TObjectPtr<UNiagaraSystem> TrailVFX;
	
	TWeakObjectPtr<UNiagaraComponent> SpawnedTrail;
};

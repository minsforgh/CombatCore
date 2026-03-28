
#include "Combat/AnimNotifyState_ConsumeWindow.h"
#include "CombatComponent.h"

void UAnimNotifyState_ConsumeWindow::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                                 float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	
	if (AActor* Owner = MeshComp->GetOwner())
	{
		if (UCombatComponent* CombatComponent = Owner->FindComponentByClass<UCombatComponent>())
		{
			CombatComponent->OnConsumeWindowEnter();
		}
	}
}

void UAnimNotifyState_ConsumeWindow::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (AActor* Owner = MeshComp->GetOwner())
	{
		if (UCombatComponent* CombatComponent = Owner->FindComponentByClass<UCombatComponent>())
		{
			CombatComponent->OnConsumeWindowExit();
		}
	}

}

FString UAnimNotifyState_ConsumeWindow::GetNotifyName_Implementation() const
{
	return FString("Consume Window");
}

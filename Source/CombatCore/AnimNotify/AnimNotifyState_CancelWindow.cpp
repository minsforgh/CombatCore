#include "AnimNotify/AnimNotifyState_CancelWindow.h"
#include "Combat/CombatComponent.h"

void UAnimNotifyState_CancelWindow::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	
	if (AActor* Owner = MeshComp->GetOwner())
	{
		if (UCombatComponent* CombatComponent = Owner->FindComponentByClass<UCombatComponent>())
		{
			CombatComponent->OnCancelWindowEnter();
		}
	}
}

void UAnimNotifyState_CancelWindow::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	
	if (AActor* Owner = MeshComp->GetOwner())
	{
		if (UCombatComponent* CombatComponent = Owner->FindComponentByClass<UCombatComponent>())
		{
			CombatComponent->OnCancelWindowExit();
		}
	}
}

FString UAnimNotifyState_CancelWindow::GetNotifyName_Implementation() const
{
	return FString("Cancel Window");
}

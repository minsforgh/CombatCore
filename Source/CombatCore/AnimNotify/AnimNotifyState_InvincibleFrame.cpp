
#include "AnimNotify/AnimNotifyState_InvincibleFrame.h"
#include "Combat/CombatComponent.h"

void UAnimNotifyState_InvincibleFrame::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	
	if (AActor* Owner = MeshComp->GetOwner())
	{
		if (UCombatComponent* CombatComponent = Owner->FindComponentByClass<UCombatComponent>())
		{
			CombatComponent->OnInvincibleFrameBegin();
		}
	}
}

void UAnimNotifyState_InvincibleFrame::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	
	if (AActor* Owner = MeshComp->GetOwner())
	{
		if (UCombatComponent* CombatComponent = Owner->FindComponentByClass<UCombatComponent>())
		{
			CombatComponent->OnInvincibleFrameEnd();
		}
	}
}

FString UAnimNotifyState_InvincibleFrame::GetNotifyName_Implementation() const
{
	return FString("InvincibleFrame");
}

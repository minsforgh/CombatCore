
#include "Combat/AnimNotify_ComboReset.h"
#include "Combat/CombatComponent.h"

void UAnimNotify_ComboReset::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
	if (AActor* Owner = MeshComp->GetOwner())
	{
		if (UCombatComponent* CombatComponent = Owner->FindComponentByClass<UCombatComponent>())
		{
			CombatComponent->OnComboResetNotify();
		}
	}
}

FString UAnimNotify_ComboReset::GetNotifyName_Implementation() const
{
	return FString("Combo Reset");
}
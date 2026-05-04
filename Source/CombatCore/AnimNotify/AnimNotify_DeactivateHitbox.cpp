
#include "AnimNotify/AnimNotify_DeactivateHitbox.h"
#include "Combat/HitboxManager.h"

void UAnimNotify_DeactivateHitbox::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                          const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
	if (AActor* Owner = MeshComp->GetOwner())
	{
		if (UHitboxManager* HM = Owner->FindComponentByClass<UHitboxManager>())
		{
			HM->StopDetection();
		}
	}
}

FString UAnimNotify_DeactivateHitbox::GetNotifyName_Implementation() const
{
	return FString("Deactivate Hitbox");
}

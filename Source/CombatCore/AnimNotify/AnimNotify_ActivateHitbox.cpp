
#include "AnimNotify/AnimNotify_ActivateHitbox.h"
#include "Combat/HitboxManager.h"

void UAnimNotify_ActivateHitbox::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                        const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
	if (AActor* Owner = MeshComp->GetOwner())
	{
		if (UHitboxManager* HM = Owner->FindComponentByClass<UHitboxManager>())
		{
			HM->StartDetection();
		}
	}
}

FString UAnimNotify_ActivateHitbox::GetNotifyName_Implementation() const
{
	return FString("Activate Hitbox");
}

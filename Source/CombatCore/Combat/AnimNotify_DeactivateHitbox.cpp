
#include "Combat/AnimNotify_DeactivateHitbox.h"

void UAnimNotify_DeactivateHitbox::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
	if (AActor* Owner = MeshComp->GetOwner())
	{
		UE_LOG(LogTemp, Log, TEXT("DeactivateHitbox: %s"), *Owner->GetName());
	}
}

FString UAnimNotify_DeactivateHitbox::GetNotifyName_Implementation() const
{
	return FString("Deactivate Hitbox");
}

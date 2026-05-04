
#include "AnimNotify/AnimNotify_FireProjectile.h"
#include "Character/RangedEnemyCharacter.h"

void UAnimNotify_FireProjectile::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
	if (AActor* Owner = MeshComp->GetOwner())
	{
		if (ARangedEnemyCharacter* Ranged = Cast<ARangedEnemyCharacter>(Owner))
		{
			Ranged->FireProjectile();
		}
	}
}

FString UAnimNotify_FireProjectile::GetNotifyName_Implementation() const
{
	return FString(TEXT("Fire Projectile"));
}

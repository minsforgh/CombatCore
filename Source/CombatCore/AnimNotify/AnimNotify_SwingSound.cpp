
#include "AnimNotify/AnimNotify_SwingSound.h"
#include "Kismet/GameplayStatics.h"

void UAnimNotify_SwingSound::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
	if (!SwingSound) return;
	
	if (AActor* Owner = MeshComp->GetOwner())
	{
		UGameplayStatics::SpawnSoundAtLocation(
			Owner->GetWorld(), SwingSound, Owner->GetActorLocation());
	}
}

FString UAnimNotify_SwingSound::GetNotifyName_Implementation() const
{
	return FString("Swing Sound");
}

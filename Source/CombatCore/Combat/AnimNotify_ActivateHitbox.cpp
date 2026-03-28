// Fill out your copyright notice in the Description page of Project Settings.


#include "Combat/AnimNotify_ActivateHitbox.h"

void UAnimNotify_ActivateHitbox::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
	if (AActor* Owner = MeshComp->GetOwner())
	{
		UE_LOG(LogTemp, Log, TEXT("ActivateHitbox: %s"), *Owner->GetName());
	}
	

}

FString UAnimNotify_ActivateHitbox::GetNotifyName_Implementation() const
{
	return FString("Activate Hitbox");
}

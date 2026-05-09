
#include "AnimNotify/AnimNotifyState_SwingTrail.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

void UAnimNotifyState_SwingTrail::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (!TrailVFX || !MeshComp) return;

	if (SpawnedTrail.IsValid())
	{
		SpawnedTrail->Deactivate();
	}

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	USceneComponent* WeaponComp = Cast<USceneComponent>(
		Owner->GetDefaultSubobjectByName(TEXT("Weapon")));
	if (!WeaponComp) return;

	const FName SocketName = FName("weapon_end");

	SpawnedTrail = UNiagaraFunctionLibrary::SpawnSystemAttached(
		TrailVFX, WeaponComp, SocketName,
		FVector::ZeroVector, FRotator::ZeroRotator,
		EAttachLocation::SnapToTarget, true);
}

void UAnimNotifyState_SwingTrail::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	
	if (SpawnedTrail.IsValid())
	{
		SpawnedTrail->DeactivateImmediate();
	}
	SpawnedTrail = nullptr;
}

FString UAnimNotifyState_SwingTrail::GetNotifyName_Implementation() const
{
	return FString("Swing Trail");
}

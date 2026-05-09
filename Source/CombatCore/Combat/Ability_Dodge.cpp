
#include "Combat/Ability_Dodge.h"
#include "Combat/CombatComponent.h"
#include "Kismet/GameplayStatics.h"

void UAbility_Dodge::Activate(UCombatComponent* Comp)
{
	if (!Comp) return;

	OwnerComponent = Comp;

	Comp->ResetComboData();

	if (!DodgeMontage)
	{
		Comp->FinishAbility();
		return;
	}

	if (!PlayMontageAndTrack(DodgeMontage))
	{
		Comp->FinishAbility();
	}
	
	if (DodgeSound)
	{
		if (AActor* Owner = Comp->GetOwner())
		{
			UGameplayStatics::PlaySoundAtLocation(
				Owner->GetWorld(), DodgeSound, Owner->GetActorLocation());
		}
	}
}

ECombatState UAbility_Dodge::GetRequiredState() const
{
	return ECombatState::Dodging;
}


#include "Combat/Ability_Dodge.h"
#include "Combat/CombatComponent.h"

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
}

ECombatState UAbility_Dodge::GetRequiredState() const
{
	return ECombatState::Dodging;
}

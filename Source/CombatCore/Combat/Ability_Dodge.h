#pragma once

#include "CoreMinimal.h"
#include "Combat/CombatAbility.h"
#include "Ability_Dodge.generated.h"

class UAnimMontage;

UCLASS()
class COMBATCORE_API UAbility_Dodge : public UCombatAbility
{
	GENERATED_BODY()

public:
	virtual void Activate(UCombatComponent* Comp) override;
	virtual ECombatState GetRequiredState() const override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Dodge")
	TObjectPtr<UAnimMontage> DodgeMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = "FX")
	TObjectPtr<USoundBase> DodgeSound;
};
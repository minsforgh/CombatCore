#pragma once

#include "CoreMinimal.h"
#include "Combat/CombatTypes.h"
#include "CombatAbility.generated.h"

class UCombatComponent;
class UAnimMontage;

UCLASS(Abstract, Blueprintable)
class COMBATCORE_API UCombatAbility : public UObject
{
	GENERATED_BODY()

public:
	virtual void Activate(UCombatComponent* Comp) PURE_VIRTUAL(,);

	virtual void End(UCombatComponent* Comp);

	virtual ECombatState GetRequiredState() const PURE_VIRTUAL(, return ECombatState::Idle;);
	
	float GetStaminaCost() const { return StaminaCost;}

	bool IsOnCooldown(float CurrentTime) const;

protected:
	bool PlayMontageAndTrack(UAnimMontage* Montage);

	UFUNCTION()
	virtual void HandleTrackedMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UPROPERTY()
	TObjectPtr<UCombatComponent> OwnerComponent;

	UPROPERTY()
	TObjectPtr<UAnimMontage> TrackedMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = "Cost")
	float StaminaCost = 0.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Cost")
	float Cooldown = 0.f;
	
	float LastActivationTime = -1e6f;
};

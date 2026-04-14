#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

class UCombatComponent;
class UHitboxManager;
class UHealthComponent;

UCLASS(abstract)
class COMBATCORE_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCombatComponent> CombatComponent;
	
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UHitboxManager> HitboxManager;
	
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UHealthComponent> HealthComponent;
	
	
public:
	ABaseCharacter();

protected:
	virtual void BeginPlay() override;
	
public:
	UCombatComponent* GetCombatComponent() const {return CombatComponent;}
	UHitboxManager* GetHitboxManager() const {return HitboxManager;}
	UHealthComponent* GetHealthComponent() const {return HealthComponent;}
};

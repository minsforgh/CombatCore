
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Combat/CombatTypes.h"
#include "HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnHealthChanged, float, CurrentHealth, float, MaxHealth, float, DeltaHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeath, const FDamageInfo&, DamageInfo);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class COMBATCORE_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess="true"))
	float MaxHealth = 100.f;
	
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess="true"))
	float CurrentHealth;
	
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess="true"))
	bool bIsDead;

public:	

	UHealthComponent();
	
	UPROPERTY(BlueprintAssignable)
	FOnHealthChanged OnHealthChanged;
	
	UPROPERTY(BlueprintAssignable)
	FOnDeath OnDeath;
	
	void ApplyDamage(const FDamageInfo& DamageInfo);
	
	UFUNCTION(BlueprintCallable)
	bool IsAlive() const {return !bIsDead;}
	
	UFUNCTION(BlueprintCallable)
	float GetHealthPercent() const;
	
	UFUNCTION(BlueprintCallable)
	float GetMaxHealth() const {return MaxHealth;}
	
	UFUNCTION(BlueprintCallable)
	float GetCurrentHealth() const {return CurrentHealth;}

protected:
	virtual void InitializeComponent() override;
	virtual void BeginPlay() override;


		
};

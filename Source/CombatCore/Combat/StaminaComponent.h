
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StaminaComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnStaminaChanged, float, CurrentStamina, float, MaxStamina, float, DeltaStamina);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class COMBATCORE_API UStaminaComponent : public UActorComponent
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	float MaxStamina = 100.f;
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	float RegenRate = 20.f;
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	float RegenDelay = 1.5f;
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	float CurrentStamina;
	
	float TimeSinceLastConsume;
	bool bIsRegenerating;
	
	
public:	
	UStaminaComponent();
	
	UPROPERTY(BlueprintAssignable)
	FOnStaminaChanged OnStaminaChanged;
	
	bool ConsumeStamina(float Amount);
	
	UFUNCTION(BlueprintCallable)
	float GetCurrentStamina() const {return CurrentStamina;};
	
	UFUNCTION(BlueprintCallable)
	float GetMaxStamina() const {return MaxStamina;};
	
	UFUNCTION(BlueprintCallable)
	float GetStaminaPercent() const;
	
	UFUNCTION(BlueprintCallable)
	bool HasEnoughStamina(float Amount) const {return CurrentStamina >= Amount;}
	
protected:
	
	virtual void InitializeComponent() override;

public:	

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};

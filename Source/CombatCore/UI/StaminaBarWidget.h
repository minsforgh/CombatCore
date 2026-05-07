
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StaminaBarWidget.generated.h"

class UStaminaComponent;

UCLASS()
class COMBATCORE_API UStaminaBarWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "HealthBar")
	void BindToStaminaComponent(UStaminaComponent* InStamina);

protected:
	virtual void NativeDestruct() override;

	UFUNCTION()
	void HandleStaminaChanged(float Current, float Max, float Delta);
	
	UFUNCTION(BlueprintImplementableEvent, Category = "StaminaBar")
	void OnStaminaUpdated(float Percent, float Current, float Max);

private:
	
	TWeakObjectPtr<UStaminaComponent> BoundStamina;

	void UnbindCurrent();
};

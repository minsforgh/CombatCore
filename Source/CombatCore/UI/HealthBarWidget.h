#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthBarWidget.generated.h"

class UHealthComponent;
struct FDamageInfo;

UCLASS(Abstract)
class COMBATCORE_API UHealthBarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "HealthBar")
	void BindToHealthComponent(UHealthComponent* InHealth);

protected:
	virtual void NativeDestruct() override;

	UFUNCTION()
	void HandleHealthChanged(float Current, float Max, float Delta);

	UFUNCTION()
	void HandleDeath(const FDamageInfo& Info);

	UFUNCTION(BlueprintImplementableEvent, Category = "HealthBar")
	void OnHealthUpdated(float Percent, float Current, float Max);

	UFUNCTION(BlueprintImplementableEvent, Category = "HealthBar")
	void OnOwnerDied();

private:
	
	TWeakObjectPtr<UHealthComponent> BoundHealth;

	void UnbindCurrent();
};

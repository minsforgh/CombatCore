 #pragma once                                                                                                                        
                                                                                                                                      
#include "CoreMinimal.h"                                                                                                            
#include "Blueprint/UserWidget.h"                                                                                                   
#include "PlayerHUDWidget.generated.h"                                                                                            
                                                                                                                                    
class APlayerCharacter;
class UHealthComponent;
class UStaminaComponent;
struct FDamageInfo;

UCLASS(Abstract)
class COMBATCORE_API UPlayerHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void BindToPlayer(APlayerCharacter* Player);

protected:
	virtual void NativeDestruct() override;

	UFUNCTION()
	void HandleHealthChanged(float Current, float Max, float Delta);

	UFUNCTION()
	void HandleDeath(const FDamageInfo& Info);

	UFUNCTION()
	void HandleStaminaChanged(float Current, float Max, float Delta);

	UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
	void OnHealthUpdated(float Percent, float Current, float Max);

	UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
	void OnOwnerDied();

	UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
	void OnStaminaUpdated(float Percent, float Current, float Max);

private:
	TWeakObjectPtr<UHealthComponent> BoundHealth;
	TWeakObjectPtr<UStaminaComponent> BoundStamina;

	void UnbindAll();
};


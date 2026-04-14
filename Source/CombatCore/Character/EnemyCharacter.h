#pragma once

#include "CoreMinimal.h"
#include "Character/BaseCharacter.h"
#include "EnemyCharacter.generated.h"

class UWidgetComponent;
struct FDamageInfo;

UCLASS()
class COMBATCORE_API AEnemyCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AEnemyCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	TObjectPtr<UWidgetComponent> HealthBarWidgetComponent;
	
	UPROPERTY(EditAnywhere, Category = "UI")
	float HealthBarMaxVisibleDistance = 3000.f;
	
private:
	UFUNCTION()
	void HandleSelfDeath(const FDamageInfo& Info);
	
	void UpdateHealthBarVisibility();
	
	FTimerHandle VisibilityTimerHandle;
};

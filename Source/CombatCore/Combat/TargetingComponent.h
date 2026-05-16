
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Combat/CombatTypes.h"
#include "TargetingComponent.generated.h"

class ABaseCharacter;
class UWidgetComponent;
class UUserWidget;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class COMBATCORE_API UTargetingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	
	UTargetingComponent();
	
	void ToggleLockOn();
	void SwitchTarget(float Direction);
	
	UFUNCTION(BlueprintCallable)
	bool IsLockedOn() const { return bIsLockedOn; }
	
	UFUNCTION(BlueprintCallable)
	AActor* GetLockedTarget() const { return LockedTarget.Get(); }

protected:
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY(EditAnywhere, Category = "Targeting")
	float MaxTargetingRange = 2500.f;
	
	UPROPERTY(EditAnywhere, Category = "Targeting")
	float MaxTargetAngle = 45.f;
	
	UPROPERTY(EditAnywhere, Category = "Targeting")
	float InterpSpeed = 10.f;
	
	UPROPERTY(EditAnywhere, Category = "Targeting")
	float TargetHeightOffset = 80.f;
	
	UPROPERTY(EditAnywhere, Category = "Targeting")
	float LOSLostGracePeriod = 3.f;
	
	UPROPERTY(EditAnywhere, Category = "Targeting|UI")
	TSubclassOf<UUserWidget> LockOnIndicatorClass;
	
	TWeakObjectPtr<AActor> LockedTarget;
	TObjectPtr<UWidgetComponent> IndicatorWidgetComponent;
	float LOSLostTimer = 0.f;
	bool bIsLockedOn = false;
	
	AActor* FindBestTarget() const;
	float ScoreTarget(AActor* Candidate) const;
	TArray<AActor*> GatherCandidates() const;
	
	void LockOnTo(AActor* Target);
	void ReleaseLockOn();
	
	UFUNCTION()
	void HandleTargetDeath(const FDamageInfo& DamageInfo);
	
	bool IsTargetValid(AActor* Target) const;
	bool HasLineOfSightTo(AActor* Target) const;
	
	void UpdateCameraRotation(float DeltaTime);
	void CreateIndicatorWidget(AActor* Target);
	void DestroyIndicatorWidget();

		
};

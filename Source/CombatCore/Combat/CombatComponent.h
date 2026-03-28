// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Combat/CombatTypes.h"
#include "CombatComponent.generated.h"

class ABaseCharacter;
class UAnimMontage;
class UComboDataAsset;
class UInputBufferComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCombatStateChanged, ECombatState, OldState, ECombatState, NewState);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class COMBATCORE_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	UCombatComponent();

protected:

	virtual void BeginPlay() override;
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UPROPERTY()
	TObjectPtr<ABaseCharacter> OwnerCharacter;

	UPROPERTY()
	FCombatStateMachine StateMachine;
	
	TWeakObjectPtr<UAnimMontage> ActiveCombatMontage;
	
	TWeakObjectPtr<UComboDataAsset> ActiveComboData;
	
	TWeakObjectPtr<UInputBufferComponent> InputBufferComponent;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UComboDataAsset> LightComboData;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UComboDataAsset> HeavyComboData;
	
	int32 CurrentComboIndex = 0;
	
	bool bIsInCancelWindow = false;
	
	bool bComboAdvanceReady = false;

public:
	UPROPERTY(BlueprintAssignable)
	FOnCombatStateChanged OnCombatStateChanged;
	
	UFUNCTION(BlueprintCallable)
	ECombatState GetState() const;
	
	UFUNCTION()
	void HandleCombatInput(EInputType InputType);
	
	UFUNCTION(BlueprintCallable)
	void ResetComboData();
	
	UFUNCTION(BlueprintCallable)
	void EndCombo();

private:
	
	bool TryChangeState(ECombatState NewState);
	
	UFUNCTION()
	void OnActiveMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	
	UFUNCTION()
	void StartCombo(EInputType InputType);
	
	UFUNCTION()
	void AdvanceCombo(EInputType InputType);
	
	void TryConsumeBufferedInput();
	
public:
	void OnConsumeWindowEnter();
	
	void OnConsumeWindowExit();
	
	void OnCancelWindowEnter();
	
	void OnCancelWindowExit();
	
	void OnComboResetNotify();
};

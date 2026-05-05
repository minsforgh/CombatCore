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
class UHitboxManager;
class UHealthComponent;
class UCombatAbility;

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
	TObjectPtr<UHitboxManager> HitboxManager;
	
	UPROPERTY()
	TObjectPtr<UHealthComponent> HealthComponent;
	
	UPROPERTY()
	FCombatStateMachine StateMachine;
	
	TWeakObjectPtr<UAnimMontage> ActiveCombatMontage;
	
	TWeakObjectPtr<UAnimMontage> ActiveHitReactionMontage;
	
	TWeakObjectPtr<UComboDataAsset> ActiveComboData;
	
	TWeakObjectPtr<UInputBufferComponent> InputBufferComponent;
	
	UPROPERTY(EditAnywhere, Category = "ComboData", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UComboDataAsset> LightComboData;

	UPROPERTY(EditAnywhere, Category = "ComboData", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UComboDataAsset> HeavyComboData;

	UPROPERTY(EditAnywhere, Category = "HitReaction", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> HitReaction_Front;

	UPROPERTY(EditAnywhere, Category = "HitReaction", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> HitReaction_Back;

	UPROPERTY(EditAnywhere, Category = "HitReaction", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> HitReaction_Left;

	UPROPERTY(EditAnywhere, Category = "HitReaction", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> HitReaction_Right;
	
	UPROPERTY(EditAnywhere, Category = "Death", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> DeathMontage;
	
	int32 CurrentComboIndex = 0;
	
	bool bIsInCancelWindow = false;
	
	bool bComboAdvanceReady = false;
	
	FTimerHandle HitStopTimerHandle;
	
	UPROPERTY(EditAnywhere, Category = "HitStop", meta = (AllowPrivateAccess = "true"))
	float MaxHitStopDuration = 0.2f;
	
	bool bIsInvincible = false;
	
	FVector CachedAttackForward = FVector::ZeroVector;
	float AttackImpulseElapsedTime = 0.f;
	
	UPROPERTY()
	TMap<TSubclassOf<UCombatAbility>, TObjectPtr<UCombatAbility>> AbilityInstances;

	UPROPERTY(EditAnywhere, Category = "Ability", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UCombatAbility> DodgeAbilityClass;
	
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
	
	UFUNCTION()
	void HandleHitDetected(const FHitResult& HitResult, AActor* HitActor);
	
	UFUNCTION()
	void ReceiveDamage(const FDamageInfo& DamageInfo);
	
	void ApplyHitStop(float Duration);
	
	bool IsInvincible() const {return bIsInvincible;}
	
	void ExecuteAbility(TSubclassOf<UCombatAbility> AbilityClass);
	
	void FinishAbility();
	
	TSubclassOf<UCombatAbility> GetDodgeAbilityClass() const {return DodgeAbilityClass;}
	
	FVector GetAttackImpulseVelocity(float DeltaTime);

private:
	
	bool TryChangeState(ECombatState NewState);
	
	UFUNCTION()
	void OnActiveMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	
	UFUNCTION()
	void StartCombo(EInputType InputType);
	
	UFUNCTION()
	void AdvanceCombo(EInputType InputType);
	
	void TryConsumeBufferedInput();
	
	EHitDirection CalcHitDirection(const FVector& InstigatorLocation) const;
	
	void PlayHitReaction(EHitDirection Direction);
	
	UCombatAbility* GetOrCreateAbility(TSubclassOf<UCombatAbility> AbilityClass);
	
public:
	void OnConsumeWindowEnter();
	
	void OnConsumeWindowExit();
	
	void OnCancelWindowEnter();
	
	void OnCancelWindowExit();
	
	void OnComboResetNotify();
	
	void OnInvincibleFrameBegin();
	
	void OnInvincibleFrameEnd();
};

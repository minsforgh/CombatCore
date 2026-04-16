#pragma once

#include "CoreMinimal.h"
#include "CombatTypes.generated.h"

class UAnimMontage;

UENUM(BlueprintType)
enum class ECombatState : uint8
{
	Idle,
	Attacking,
	Dodging,
	HitStun,
	Dead
};

UENUM(BlueprintType)
enum class EInputType : uint8
{
	Light,
	Heavy
};

UENUM(BlueprintType)
enum class EHitDirection : uint8
{
	Forward,
	Back,
	Left,
	Right
};

USTRUCT(BlueprintType)
struct FComboStep
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimMontage> Montage;
	
	UPROPERTY(EditAnywhere)
	TMap<EInputType, int32> BranchMap;
	
	UPROPERTY(EditAnywhere)
	float Damage = 10.f;
	
	UPROPERTY(EditAnywhere)
	float KnockbackForce = 300.f;
	
};

USTRUCT(BlueprintType)
struct FDamageInfo
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	float Damage = 0.f;
	
	UPROPERTY(EditAnywhere)
	float KnockbackForce = 0.f;
	
	UPROPERTY()
	FVector KnockbackDirection = FVector::ZeroVector;
	
	TWeakObjectPtr<AActor> Instigator;
	
	UPROPERTY(EditAnywhere)
	float HitStopDuration = 0.f;
	
	UPROPERTY()
	FHitResult HitResult;
};

USTRUCT(BlueprintType)
struct FCombatStateMachine
{
	GENERATED_BODY()

private:
	UPROPERTY()
	ECombatState CurrentState = ECombatState::Idle;

public:
	ECombatState GetState() const
	{
		return CurrentState;
	}

	bool CanTransition(ECombatState NewState, bool bIsInCancelWindow = false) const;
	bool TryTransition(ECombatState NewState, bool bIsInCancelWindow = false);
};

inline bool FCombatStateMachine::CanTransition(ECombatState NewState, bool bIsInCancelWindow) const
{
	switch (CurrentState)
	{
	case ECombatState::Idle:
		return NewState == ECombatState::Attacking 
		|| NewState == ECombatState::Dodging
		|| NewState == ECombatState::HitStun
		|| NewState == ECombatState::Dead;
		
	case ECombatState::Attacking:
		// 공격 중 회피는 CancelWindow 구간에서만 허용
		if (NewState == ECombatState::Dodging) return bIsInCancelWindow;
		return NewState == ECombatState::Idle || NewState == ECombatState::HitStun || NewState == ECombatState::Dead;
		
	case ECombatState::Dodging:
		return NewState == ECombatState::Idle || NewState == ECombatState::HitStun || NewState == ECombatState::Dead;
		
	case ECombatState::HitStun:
		return NewState == ECombatState::Idle || NewState == ECombatState::Dead || NewState == ECombatState::HitStun;
		
	case ECombatState::Dead:
		return false;
		
	default:
		return false;
	}
}

inline bool FCombatStateMachine::TryTransition(ECombatState NewState, bool bIsInCancelWindow)
{
	if (!CanTransition(NewState, bIsInCancelWindow))
	{
		return false;
	}
	CurrentState = NewState;
	return true;
}

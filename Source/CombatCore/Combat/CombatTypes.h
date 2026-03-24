#pragma once

#include "CoreMinimal.h"
#include "CombatTypes.generated.h"

UENUM(BlueprintType)
enum class ECombatState : uint8
{
	Idle,
	Attacking,
	Dodging,
	HitStun,
	Dead
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
		return NewState == ECombatState::Attacking || NewState == ECombatState::Dodging;
		
	case ECombatState::Attacking:
		// 공격 중 회피는 CancelWindow 구간에서만 허용
		if (NewState == ECombatState::Dodging) return bIsInCancelWindow;
		return NewState == ECombatState::HitStun || NewState == ECombatState::Dead;
		
	case ECombatState::Dodging:
		return NewState == ECombatState::Idle || NewState == ECombatState::HitStun || NewState == ECombatState::Dead;
		
	case ECombatState::HitStun:
		return NewState == ECombatState::Idle || NewState == ECombatState::Dead;
		
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

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CombatTypes.h"
#include "ComboDataAsset.generated.h"


UCLASS()
class COMBATCORE_API UComboDataAsset : public UDataAsset
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere)
	TArray<FComboStep> ComboSteps;

public:
	const FComboStep* GetStep(int32 Index) const;
};

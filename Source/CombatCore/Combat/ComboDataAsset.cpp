// Fill out your copyright notice in the Description page of Project Settings.


#include "Combat/ComboDataAsset.h"

const FComboStep* UComboDataAsset::GetStep(int32 Index) const
{
	if (!ComboSteps.IsValidIndex(Index)) return nullptr;
	
	return &ComboSteps[Index];
}

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyAITypes.generated.h"

UENUM(BlueprintType)
enum class EEnemyAIState : uint8
{
	Idle UMETA(DisplayName = "Idle"),
	Chase UMETA(DisplayName = "Chase"),
	Wait UMETA(DisplayName = "Wait"),
	Attack UMETA(DisplayName = "Attack")
};


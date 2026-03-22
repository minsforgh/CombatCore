// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

/*class UCombatComponent;
class UHitboxManager;
class UHealthComponent;*/

UCLASS(abstract)
class COMBATCORE_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()
	
	/*UPROPERTY()
	UCombatComponent* CombatComponent;
	
	UPROPERTY()
	UHitboxManager* HitboxManager;
	
	UPROPERTY()
	UHealthComponent* HealthComponent;
	*/
	
public:
	// Sets default values for this character's properties
	ABaseCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
};

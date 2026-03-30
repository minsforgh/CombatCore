
#include "Character/BaseCharacter.h"
#include "Combat/CombatComponent.h"

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	
	CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));

}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	CombatComponent = FindComponentByClass<UCombatComponent>();

	
}


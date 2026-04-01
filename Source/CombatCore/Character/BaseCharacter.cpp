
#include "Character/BaseCharacter.h"
#include "Combat/CombatComponent.h"
#include "Combat/HitboxManager.h"
#include "Combat/HealthComponent.h"

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	
	CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	HitboxManager = CreateDefaultSubobject<UHitboxManager>(TEXT("HitboxManager"));
	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	// 직렬화 문제 해결용 백업 코드
	CombatComponent = FindComponentByClass<UCombatComponent>();
	HitboxManager = FindComponentByClass<UHitboxManager>();
	HealthComponent = FindComponentByClass<UHealthComponent>();

	
}


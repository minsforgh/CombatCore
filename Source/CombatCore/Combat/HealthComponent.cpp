
#include "Combat/HealthComponent.h"
#include "Combat/CombatTypes.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;
}

void UHealthComponent::InitializeComponent()
{
	Super::InitializeComponent();
	CurrentHealth = MaxHealth;
}

void UHealthComponent::ApplyDamage(const FDamageInfo& DamageInfo)
{
	if (bIsDead) return;
	
	const float ActualDamage = FMath::Min(DamageInfo.Damage, CurrentHealth);
	CurrentHealth -= ActualDamage;
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth, -ActualDamage);
	
	if (CurrentHealth <= 0)
	{
		bIsDead = true;
		OnDeath.Broadcast(DamageInfo);
	}
}


float UHealthComponent::GetHealthPercent() const
{
	if (MaxHealth <= 0.f) return 0.f;
	return CurrentHealth / MaxHealth;
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();
}




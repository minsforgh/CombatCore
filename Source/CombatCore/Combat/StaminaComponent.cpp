#include "Combat/StaminaComponent.h"

UStaminaComponent::UStaminaComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;

}

void UStaminaComponent::InitializeComponent()
{
	Super::InitializeComponent();
	CurrentStamina = MaxStamina;
	TimeSinceLastConsume = 0.f;
	bIsRegenerating = false;
	SetComponentTickEnabled(false);
}

bool UStaminaComponent::ConsumeStamina(float Amount)
{
	if (Amount <= 0.f) return true;
	if (CurrentStamina < Amount) return false;
	
	CurrentStamina -= Amount;
	TimeSinceLastConsume = 0.f;
	bIsRegenerating = false;
	SetComponentTickEnabled(true);
	OnStaminaChanged.Broadcast(CurrentStamina, MaxStamina, -Amount);
	return true;
}

void UStaminaComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (CurrentStamina >= MaxStamina)
	{
		SetComponentTickEnabled(false);
		return;
	}
	
	if (!bIsRegenerating)
	{
		TimeSinceLastConsume += DeltaTime;
		if (TimeSinceLastConsume >= RegenDelay)
		{
			bIsRegenerating = true;
		}
		return;
	}
	
	const float OldStamina = CurrentStamina;
	CurrentStamina = FMath::Min(CurrentStamina + RegenRate * DeltaTime, MaxStamina);
	const float Delta = CurrentStamina - OldStamina;
	if (Delta > 0.f)
	{
		OnStaminaChanged.Broadcast(CurrentStamina, MaxStamina, Delta);
	}
}

float UStaminaComponent::GetStaminaPercent() const
{
	if (MaxStamina <= 0.f) return 0.f;
	return CurrentStamina / MaxStamina;
}


